#ifndef _JMP_H_
#define _JMP_H_

/**
 *  @file       jmp.h
 *  @brief      setjmp/longjmp Implementierung
 *
 * Dies ist eine Einfache Implementierung der C Standardfunktionen
 * @c setjmp() und @c longjmp(). Im Unterschied zu diesen Standardfunktionen
 * erlaubt diese Implementierung zusaetzlich das gezielte Manipulieren
 * des Stackpointers und des Befehlszeigers eines geretteten Registersatzes.
 * Diese Funktionalitaet ist noetig, um neue Threads erzeugen zu koennen.
 *
 */

/** Dies ermoeglicht das Kompilieren dieses Codes auch mit nicht-GNU compilern */
#ifndef __GNUC__
#define __attribute__(x)
#define __inline__
#endif


#ifdef __i386__
/**
 * This part is specific to the intel x86 machine architecture
 */

/**
 * @brief Indizes der Elemente des @c __jmp_buf arrays
 *
 * Achtung: Diese definitionen muessen zum Code der Implementierung passen!
 */
#define JB_BX  0
#define JB_SI  1
#define JB_DI  2
#define JB_BP  3
#define JB_SP  4
#define JB_PC  5
#define JB_SIZE 6

/**
 * @struct __jmp_buf
 * @brief  Registerkontext
 *
 * In dieser Struktur werden die register des Prozessors
 * gespeichert. Fuer alle Architecturen muss __jmp_buf[JB_SP]
 * der Stackpointer und __jmp_buf[JB_PC] der Befehlszaehler des
 * Aufrufers sein.
 */
typedef int __jmp_buf[JB_SIZE];


/**
 * @brief Manipulieren des Programmzaehlers
 *
 *
 * @param env Registerkontext auf dem gearbeitet wird
 * @param pc  Wert, der in den Programmzaehler geladen werden soll
 * @retval nichts
 */
static __inline__ void jmp_buf_set_pc(__jmp_buf env, void (*pc)())
{
    env[JB_PC] = (int)pc;
}

/**
 * @brief Manipulieren des Stackpointers
 *
 *
 * @param env Registerkontext auf dem gearbeitet wird
 * @param sp  Wert, der in den Stackpointer geladen werden soll
 * @retval nichts
 */
static __inline__ void jmp_buf_set_sp(__jmp_buf env, void *sp)
{
    env[JB_SP] = (int)sp;
}

#else

#error "Jmp.h muss an diese Architektur angepasst werden."

#endif


/**
 * @brief Registerkontext abspeichern
 *
 * @param env @c __jmp_buf Struktur in die gespeichert werden soll
 * @retval 0 Wenn direkt zurueckgekehrt wird
 * @retval nichtnull wenn ueber @c longjmp() zurueckgekehrt wird
 */
int __setjmp(__jmp_buf env);

/**
 * @brief Mit @c setjmp() geretteten Registerkontext wiederherstellen
 *
 * @param  env @c __jmp_buf Struktur mit dem herzustellenden Registerkontext
 * @param  val Rueckgabewert fuer @c setjmp()
 * @retval keiner
 */
void __longjmp(__jmp_buf env, int val);


#endif /* _JMP_H_ */
