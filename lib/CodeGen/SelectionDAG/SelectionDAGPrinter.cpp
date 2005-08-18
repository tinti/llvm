//===-- SelectionDAGPrinter.cpp - Implement SelectionDAG::viewGraph() -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This implements the SelectionDAG::viewGraph method.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Function.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Config/config.h"
#include <fstream>
using namespace llvm;

namespace llvm {
  template<>
  struct DOTGraphTraits<SelectionDAG*> : public DefaultDOTGraphTraits {
    static std::string getGraphName(const SelectionDAG *G) {
      return G->getMachineFunction().getFunction()->getName();
    }

    static bool renderGraphFromBottomUp() {
      return true;
    }

    static std::string getNodeLabel(const SDNode *Node,
                                    const SelectionDAG *Graph);
    static std::string getNodeAttributes(const SDNode *N) {
      return "shape=Mrecord";
    }

    static void addCustomGraphFeatures(SelectionDAG *G,
                                       GraphWriter<SelectionDAG*> &GW) {
      GW.emitSimpleNode(0, "plaintext=circle", "GraphRoot");
      GW.emitEdge(0, -1, G->getRoot().Val, -1, "");
    }
  };
}

std::string DOTGraphTraits<SelectionDAG*>::getNodeLabel(const SDNode *Node,
                                                        const SelectionDAG *G) {
  std::string Op = Node->getOperationName(G);

  for (unsigned i = 0, e = Node->getNumValues(); i != e; ++i)
    if (Node->getValueType(i) == MVT::Other)
      Op += ":ch";
    else
      Op = Op + ":" + MVT::getValueTypeString(Node->getValueType(i));
    
  if (const ConstantSDNode *CSDN = dyn_cast<ConstantSDNode>(Node)) {
    Op += ": " + utostr(CSDN->getValue());
  } else if (const ConstantFPSDNode *CSDN = dyn_cast<ConstantFPSDNode>(Node)) {
    Op += ": " + ftostr(CSDN->getValue());
  } else if (const GlobalAddressSDNode *GADN =
             dyn_cast<GlobalAddressSDNode>(Node)) {
    Op += ": " + GADN->getGlobal()->getName();
  } else if (const FrameIndexSDNode *FIDN = dyn_cast<FrameIndexSDNode>(Node)) {
    Op += " " + itostr(FIDN->getIndex());
  } else if (const ConstantPoolSDNode *CP = dyn_cast<ConstantPoolSDNode>(Node)){
    Op += "<" + utostr(CP->getIndex()) + ">";
  } else if (const BasicBlockSDNode *BBDN = dyn_cast<BasicBlockSDNode>(Node)) {
    Op = "BB: ";
    const Value *LBB = (const Value*)BBDN->getBasicBlock()->getBasicBlock();
    if (LBB)
      Op += LBB->getName();
    //Op += " " + (const void*)BBDN->getBasicBlock();
  } else if (const RegisterSDNode *R = dyn_cast<RegisterSDNode>(Node)) {
    Op += " #" + utostr(R->getReg());
  } else if (const ExternalSymbolSDNode *ES =
             dyn_cast<ExternalSymbolSDNode>(Node)) {
    Op += "'" + std::string(ES->getSymbol()) + "'";
  } else if (const SrcValueSDNode *M = dyn_cast<SrcValueSDNode>(Node)) {
    if (M->getValue())
      Op += "<" + M->getValue()->getName() + ":" + itostr(M->getOffset()) + ">";
    else
      Op += "<null:" + itostr(M->getOffset()) + ">";
  } else if (const VTSDNode *N = dyn_cast<VTSDNode>(Node)) {
    std::cerr << ":" << getValueTypeString(N->getVT());
  }
  return Op;
}


/// viewGraph - Pop up a ghostview window with the reachable parts of the DAG
/// rendered using 'dot'.
///
void SelectionDAG::viewGraph() {
// This code is only for debugging!
#ifndef NDEBUG
  std::string Filename = "/tmp/dag." +
    getMachineFunction().getFunction()->getName() + ".dot";
  std::cerr << "Writing '" << Filename << "'... ";
  std::ofstream F(Filename.c_str());

  if (!F) {
    std::cerr << "  error opening file for writing!\n";
    return;
  }

  WriteGraph(F, this);
  F.close();
  std::cerr << "\n";

#ifdef HAVE_GRAPHVIZ
  std::cerr << "Running 'Graphviz' program... " << std::flush;
  if (system((LLVM_PATH_GRAPHVIZ " " + Filename).c_str())) {
    std::cerr << "Error viewing graph: 'Graphviz' not in path?\n";
  } else {
    system(("rm " + Filename).c_str());
    return;
  }
#endif  // HAVE_GRAPHVIZ

#ifdef HAVE_GV
  std::cerr << "Running 'dot' program... " << std::flush;
  if (system(("dot -Tps -Nfontname=Courier -Gsize=7.5,10 " + Filename
              + " > /tmp/dag.tempgraph.ps").c_str())) {
    std::cerr << "Error viewing graph: 'dot' not in path?\n";
  } else {
    std::cerr << "\n";
    system(LLVM_PATH_GV " /tmp/dag.tempgraph.ps");
  }
  system(("rm " + Filename + " /tmp/dag.tempgraph.ps").c_str());
  return;
#endif  // HAVE_GV
#endif  // NDEBUG
  std::cerr << "SelectionDAG::viewGraph is only available in debug builds on "
            << "systems with Graphviz or gv!\n";

#ifndef NDEBUG
  system(("rm " + Filename).c_str());
#endif
}
