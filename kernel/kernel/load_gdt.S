


.global load_gdt
.type load_gdt, @function

load_gdt:
/*Sto usando sintassi AT&T, con % per riferirmi a registri e mov sorgente, destinazione
    sintassi intel non usa % ed è mov dest, sorg
*/
 //passo come parametro addr gdt

    //dopo call funzione lo stack ha arg1, arg2, ..., ret_address. Uno sopra l'altro.
    //di base quindi dopo call esp punta a inizio ret_addr
    //se voglio leggere parametri, aggiugo 4byte siccome ogni elemento nella stack è lungo 4byte, con padding se serve (allineato a 4B)
    // e siccome stack cresce verso il basso devo aggiungere per salire e vedere parametri precedenti e sottrarre per andare oltre e crescere
    mov 4(%esp), %eax //4(%esp) è come [esp + 4] => esp è punto attuale stack.

    //comando asm specifco per caricare GDT nel registro GDTR della CPU
    lgdt (%eax)

    //settiamo i registri che la cpu usa con il nuovo ruolo KERNEL_DATA e KERNEL_CODE (per reg CS [code segment])
    //ogni registro viene usato dalla cpu per tipi di azioni e non può avere valore qualsiasi o valore del GDT di GRUB che può essere sovrascritto
    //mettiamo i privilegi già ora per evitare crash o eccezioni => in futuro con le syscall la cpu cambierà da sola il ruolo vedendo la GDT

    //0x10 => KERNEL DATA SEGMENT DPL
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    // 0x08 => kernel code segment dpl
    //ljmp (long jump) comando specifico per modificare CS reg, siccome CS è accoppiato con il PC, (EIP) non si può modificarlo con mov
    //solo con ljmp che primo parametro è il nuovo valore di CS  e secondo parametro è da dove EIP riprenderà il lavor odi Program Counter
    ljmp $0x08, $flush

flush:
    ret

