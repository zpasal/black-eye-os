# Black Eye OS


Exploring new concepts of OS design with 64bit processors.


# Prerequirments
* nasm
* quemu (quemu-system-x86)
* grub-mkrescue
* git :)

       
876543210987 654332109 876543210 987654321 098765432 109876543210

# LOADER BIG PAGE 2MB

               P4(9)   v  P3(9)    P2(9)          A(21)
|           |         |         |         |         |            |
| 63 - 48   | 47 - 39 | 38 - 30 | 29 - 21 |       20 - 0         |
876543210987 654332109 876543210 987654321 098765432109876543210
111111111111 111111111 111111111 111000000 000000000000000000000 FFFFFFFF80000000
111111111111 100000000 000000000 000000000 000000000000000000000 FFFF800000000000




# INTERRUPT STACK

    +------------+
+40 | %SS        |
+32 | %RSP       |
+24 | %RFLAGS    |
+16 | %CS        |
 +8 | %RIP       |
  0 | ERROR CODE | <-- %RSP
    +------------+


