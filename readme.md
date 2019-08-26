# Black Eye OS

Exploring new concepts of OS design with 64bit processors.

# Prerequirments
* nasm
* quemu (quemu-system-x86)
* grub-mkrescue
* git :)

               P4(9)   v  P3(9)    P2(9)          A(21)
|           |         |         |         |         |            |
| 63 - 48   | 47 - 39 | 38 - 30 | 29 - 21 |       20 - 0         |
876543210987 654332109 876543210 987654321 098765432109876543210

 


P4 @ 0x00000000 4K - 512 entries 1
P3 @ 0x00001000 4K - 512 entries 1
P2 @ 0x00002000 4K - 512 entries 512



1 P4 -> 1 P3 -> 512 2MiB P2 tables



    +------------+
+40 | %SS        |
+32 | %RSP       |
+24 | %RFLAGS    |
+16 | %CS        |
 +8 | %RIP       |
  0 | ERROR CODE | <-- %RSP
    +------------+