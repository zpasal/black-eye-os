# Black Eye OS


Exploring new concepts of OS design with 64bit processors.


# Prerequirments
* nasm
* quemu (quemu-system-x86)
* grub-mkrescue
* git :)


# WINDOWS MANAGER

- CREATE_WINDOW - 
       
876543210987 654332109 876543210 987654321 098765432 109876543210

# LOADER BIG PAGE 2MB

               P4(9)   v  P3(9)    P2(9)          A(21)
|           |         |         |         |         |            |
| 63 - 48   | 47 - 39 | 38 - 30 | 29 - 21 |       20 - 0         |
876543210987 654332109 876543210 987654321 098765432109876543210
             0x000         0         0              0
000000000000 000000000 000000000 000000000 000000000000000000000 0000000000000000 - USER SPACE
             0x100         0         0              0
111111111111 100000000 000000000 000000000 000000000000000000000 FFFF800000000000 - KERNEL
             0x180         0         0              0
111111111111 110000000 000000000 000000000 000000000000000000000 FFFFC00000000000 - VIDEO
             0x180         0        256             0
111111111111 110000000 000000000 100000000 000000000000000000000 FFFFC00020000000 - TMP PROCESS MAPPING
             0x180         0        511             0
111111111111 110000000 000000000 111111111 000000000000000000000 FFFFC0003FE00000 - TMP MAPPING (alloc frame)


KERNEL_START         FFFF800000000000
                          KERNEL
KERNEL_END           FFFF800000XXXXXX
													BITSET
HEAP                 FFFF800000XXXXXX + 0x200000 (2MB)
											  KERNEL HEAP


HEAP:



# INTERRUPT STACK

    +------------+
+40 | %SS        |
+32 | %RSP       |
+24 | %RFLAGS    |
+16 | %CS        |
 +8 | %RIP       |
  0 | ERROR CODE | <-- %RSP
    +------------+




