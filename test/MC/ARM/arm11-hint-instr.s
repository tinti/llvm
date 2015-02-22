@ RUN: not llvm-mc -triple=armv6 -mcpu=arm1136jzf-s -o /dev/null < %s 2>&1 \
@ RUN:     | FileCheck --check-prefix=CHECK-ERRORS %s
@ RUN: llvm-mc -triple=armv6k -mcpu=mpcore -o /dev/null < %s
@ RUN: llvm-mc -triple=armv6k -mcpu=arm1176jzf-s -o /dev/null < %s
@ RUN: llvm-mc -triple=armv6t2 -mcpu=arm1156t2f-s -o /dev/null < %s

@------------------------------------------------------------------------------
@ SEV/WFE/WFI/YIELD - are not supported pre v6K
@------------------------------------------------------------------------------
        sev
        wfe
        wfi
        yield

@ CHECK-ERRORS: error: instruction requires: armv6k
@ CHECK-ERRORS: sev
@ CHECK-ERRORS: ^
@ CHECK-ERRORS: error: instruction requires: armv6k
@ CHECK-ERRORS: wfe
@ CHECK-ERRORS: ^
@ CHECK-ERRORS: error: instruction requires: armv6k
@ CHECK-ERRORS: wfi
@ CHECK-ERRORS: ^
@ CHECK-ERRORS: error: instruction requires: armv6k
@ CHECK-ERRORS: yield
@ CHECK-ERRORS: ^
