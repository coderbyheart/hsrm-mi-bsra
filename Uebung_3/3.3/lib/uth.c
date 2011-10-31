/**
 *  @file       uth.c
 *  @brief      Micro Thread Bibliothek
 *
 * Dies ist eine Bibliothek zur Unterstuetzung von einfachem, @b kooperativem Multi-Threading
 * auf Anwenderebene.
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/jmp.h"
#include "../include/uth.h"
#include "../include/uthconfig.h"


/**
 * Stack-groesse in Worten: STACK_PER_THREAD
 * gibt die Groesse in Bytes an, also....
 */
#define STACKSIZE (STACK_PER_THREAD/sizeof(int))

/**
 * @brief Thread-Verwaltungsstruktur:
 *
 * Wir brauchen:
 * 1) eine __jmp_buf Struktur zum Retten des Prozessorkontextes
 * 2) Platz fuer den Stack
 *
 * Ausserdem verwenden wir noch ein Flag, mit dem die Struktur
 * als "belegt" markiert wird.
 * Wir packen alles in eine gemeinsame Struktur. Das ist zwar
 * nicht zwingend, aber es vereinfacht die Dinge ein wenig..
 *
 * Was der Aufrufer als "Thread-Handle" wahrnimmt, ist innerhalb der Micro
 * Thread Bibliothek ein Zeiger auf die Thread-Struktur des betreffenden
 * Threads. Diese Struktur ist ausserhalb der Bibliothek nicht bekannt,
 * (und soll es auch nicht sein), deshalb geben wir Zeiger auf sie nach
 * aussen immer nur als void-Zeiger.
 *
 * Die Bibliothek alloziert in @c uth_init() ein Array solcher Strukturen,
 * eine pro Thread.
 */
struct thread
{
    /** Flag: wenn != 0 -> Struktur ist belegt      */
    int busy;

    /** Hier werden die Prozessorregister gerettet */
    __jmp_buf context;

    /** Speicherplatz fuer den Stack des Thread     */
    int stack[STACKSIZE];
};


/*
 * Statische Variablen: Das hier ist nur ein Vorschlag!
 */
/** Zeiger auf thread-Datenstrukturen */
static struct thread *threads;

/** Maximale Anzahl der Threads       */
static int            num_threads;

/** Kontext des main()-Programmes     */
static __jmp_buf      main_context;

/**
 * Lokale Hilfsprozeduren:
 */

/**
 * @brief Initialisieren eines Pozessorkontextes
 *
 * Bei der "normalen" Verwendung von @c setjmp() und @c longjmp() wird die @c jmp_buf
 * Struktur mit Hilfe der Funktion @c setjmp() mit Werten versehen. Im Gegensatz dazu
 * erzeugen wir hier "kuenstlich" (d.h. ohne @c setjmp() ) eine @c jmp_buf Struktur
 * indem wir sie "von Hand" mit Werten fuer Stackpointer und Programmzaehler versehen.
 * Ein spaeteres @c __longjmp() mit dieser Struktur wird dazu fuehren, dass der Code
 * ab Adresse @c entry ausgefuehrt wird, wobei der Stackpointer des Prozessors in den
 * Bereich @c stack[] der zugehoerigen Thread-Struktur zeigt.
 *
 * @param thr Zeiger auf die Thread-Struktur, deren Kontext zu initialisieren ist
 * @param entry Zeiger auf die Stelle im Code (Funktionszeiger), wo der Thread mit der Ausfuehrung beginnen soll
 * @retval nichts
 */
static void setup_context(struct thread *thr, void (*entry)(void))
{
    /**
     * Erst mal alles mit Nullen vollschreiben. Das waere zwar
     * eigenlich gar nicht noetig, aber so bekommt der neue
     * Thread alle Prozessorregister "sauber" mit Nullen serviert
     * (anderenfalls waeren sie undefiniert).
     */
    memset(thr->context, 0, sizeof(thr->context));

    /**
     * Setze den Programmzaehler in @c thr->context zu @c entry
     */
    jmp_buf_set_pc(thr->context, entry);

    /**
     * Knifflig: Der Stack waechst nach "unten", d.h. zu kleiner
     * werdenden Adressen hin. Der Stackpointer muss deshalb
     * initial auf die @b hoechste Adresse im Bereich des Arrays
     * @c thr->stack[] gesetzt werden, damit der Stack moeglichst
     * weit nach "unten" wachsen kann. Diese hoechste Adresse ist die Adresse
     * des @b letzten Elementes dieses Arrays, also @c &thr->stack[STACKSIZE-1]
     */
	/* Hier wird das Argument für die Methode uth_thread_terminate im stack abgelegt */
    thr->stack[STACKSIZE-1] = (void *)thr;
	/* Als für den Rücksprung wird die uth_thread_terminate definiert */
    thr->stack[STACKSIZE-3] = uth_thread_terminate;
    jmp_buf_set_sp(thr->context, &thr->stack[STACKSIZE-3]);

    /**
     * So, der Kontext ist jetzt initalisiert.
     */
}


/**
 * @brief Thread-Umschaltung
 *
 * Das ist die eigentliche Thread-Umschaltung: Der Zustand des bisher laufenden
 * Thread muss abgespeichert werden (mit @c setjmp() ), und der zuletzt gerettete
 * Zustand des naechsten Thread muss mit @c longjmp() wiederhergestellt werden.
 *
 * @param current Zeiger auf die Thread-Struktur des bisher laufenden Thread
 * @param next Zeiger auf die Thread-Struktur des Thread, zu dem gewechselt werden soll
 * @retval nichts
 */
static void thread_switch(struct thread *current, struct thread *next)
{
    if(!__setjmp(current->context))
    {
        __longjmp(next->context, 1);
        assert(0);  /* hier kommen wir niemals hin */
    }
    /*
     * Wenn wir hier sind, ist @c current wieder der aktuell laufende Thread,
     * dessen Zustand soeben mit __longjmp() wiederhergestellt wurde.
     */
}

/**
 * @brief Finde den naechsten belegten Thread
 *
 * Die Thread-Strukturen sind als Array angeordnet. Ausgehend von einer durch
 * den Parameter @c thr gegebenen Thread-Struktur soll die naechste Thread-Struktur
 * ermittelt werden, deren Flag @c busy nicht Null ist, (die also belegt ist).
 * Wird dabei das Ende des Arrays erreicht, so wird die Suche am Anfang fortgesetzt.
 * (Alternativ liesse sich das auch sehr schoen mit verketteten Listen realisieren).
 *
 *
 * @param thr Zeiger auf die Thread-Struktur des bisher laufenden Thread
 * @retval next Zeiger auf die Thread-Struktur des naechsten, auf @c thr folgenden
 *              belegten Thread: das kann auch @c thr selbst sein.
 */
static struct thread* next_busy_thread(struct thread* thr)
{
    int i;

    /** ermittle den Index des Threads @c thr */
    for(i = 0; &threads[i] != thr; i++)
        assert(i != num_threads); /** einer der @c threads[i] @b muss @c thr sein!  */
    assert(thr->busy);

    /** Ausgehend von @c i , suche den naechsten Thread mit threads[i].busy != 0 */
    do
    {   /**
         * Wenn wir ans Ende des Arrays stossen,
         * fangen wir vorne wieder an
         */
        if(++i == num_threads)
            i = 0;
    }
    while(!threads[i].busy);
    return(&threads[i]);
}


/**
 * Exportierte Funtkionen:
 */

/**
 * @brief Erzeugen eines Thread
 *
 * Diese Funktion erzeugt einen neuen Thread. Die noetigen Datenstrukturen werden
 * angelegt und ein @b handle wird zurueckgeliefert, mit dem der Thread kuenftig
 * referenziert werden kann. Falls nicht genuegend Ressourcen vorhanden sind, um
 * den Thread anzulegen (z.B. zu wenig Speicher fuer den Stack), so liefert die
 * Funktion @c NULL zurueck.
 *
 * @param entry Eintrittspunkt des neuen Thread
 * @retval handle Handle zum Referenzieren des Threads
 * @retval NULL Erzeugen des Threads schlug fehl
 */
void* uth_thread_create(void (*entry)(void))
{
    /**
     * Was ist zu tun?
     * 1) Eine freie Thread-Struktur finden (eine mit threads[i].busy == 0)
     * 2) Die Struktur als "belegt" markieren
     * 3) Den darin enthaltenen Kontext initialisieren, (mit @c setup_context() )
     * 4) einen void-Zeiger auf die Thread-Stuktur als "handle" zurueckgeben
     */
	int i;
	for(i = 0; i < num_threads; i++) {
		if(threads[i].busy == 0) {
			threads[i].busy = 1;
			setup_context(&threads[i], entry);
			return((void*)&threads[i]);
		}
	}
	return NULL;
}

/**
 * @brief Loeschen eines Thread
 *
 * Diese Funktion loescht einen Thread. Die verwendeten Datenstrukturen werden
 * freigegeben. Der zu loeschende wird durch sein @b handle referenziert. Es
 * kann sich hierbei auch um den Thread des Aufrufers handeln. Wenn der letzte
 * Thread der Micro Thread Bibliothek geloescht wird, kehrt die Funktion @c uth_init()
 * zu ihrem Aufrufer zurueck.
 *
 * @param handle Handle des zu loeschenden Threads
 * @retval 0 Thread erfolgreich geloescht
 * @retval -1 @c handle identifiziert keinen bekannten Thread
 */

int uth_thread_terminate(void *handle)
{
    /**
     * Was ist zu tun?
     * 1) Die durch @c handle bezeichnete Thread-Struktur finden
     * 2) Falls der Thread @b nicht selbst der Aufrufer dieser Funktion ist
     *    (d.h. @c uth_get_handle() != @c handle), Thread-Struktur freigeben -> fertig
     * 3) Falls der zu terminierende Thread selbst die Funktion aufgerufen
     *    hat ("Selbstmord"), muss zusaetzlich zum naechsten Thread gewechselt
     *    werden:
     *    a) Falls dieser naechste Thread ein Anderer als der Aufrufer ist,
     *       Thread-Struktur freigeben und zum nachsten Thread wechseln
     *       (mit @c thread_switch() ) -> fertig
     *    b) Falls es aber nur noch einen einzigen Thread gibt, d.h. falls
     *       der letzte Thread gerade im Begriff ist, sich zu beenden, muss
     *       das ganze Thread-System beendet werden. Fuer diesen Fall ist
     *       vorgesehen, dass @c uth_init() zu seinem Aufrufer zurueckkehrt.
     *       Dazu wurde in @c uth_init() der Registerkontext des Aufrufers
     *       mit @c __setjmp() in @c main_context gespeichert. Dieser Kontext
     *       muss somit mit @c __longjmp() aktiviert werden.
     */
	int i = 0;
	struct thread *mythread;
	struct thread *next;
	for(; i < num_threads; i++) {
		if((void*)&threads[i] == handle) {
			/* Thread gefunden */
			mythread = handle;
		}
	}
	if (!mythread) return -1;
	if (mythread != uth_get_handle()) {
		/* Ruft sich selbst auf */
		mythread->busy = 0;
			printf(">>> Terminiere mich selbst.\n");
	} else {
		next = next_busy_thread(mythread);
		/* Einen anderen Thread beenden */
		if (mythread != next) {
			/* Wechsel zum nächsten Thread */
			mythread->busy = 0;
			thread_switch(mythread, next);
		} else {
			printf(">>> Terminiere mich selbst.\n");
			__longjmp(main_context, 1);
		}
	}
	return 0;
	
}


/**
 * @brief Eigenes Thread-Handle ermitteln
 *
 * Diese Funktion liefert den Handle des Threads des Aufrufers zurueck. Die Identifikation
 * des Threads erfolgt anhand des Stackpointers des Aufrufers. Dieser muss innerhalb eines
 * fuer jeden Thread individuellen Adressbereiches liegen.
 *
 * @retval handle Handle des Threads des Aufrufers
 * @retval 0 Aufruf von einem Thread, der nicht unter der Verwaltung der Micro Thread Bibliothek liegt
 */
void* uth_get_handle(void)
{
    /**
     * Was ist zu tun?
     * 1) einen Zeiger auf eine lokale Variable beschaffen
     *    -> diese Variable muss innerhalb des Stack-Bereiches
     *       des aufrufenden Threads liegen.
     * 2) die Adresse dieser Variablen mit den Adessbereichs-Grenzen
     *    der Stacks aller bekannter Threads vergleichen.
     * 3) Derjenige, in dessen Stackbereich die Variable liegt, ist der Gesuchte.
     */

    /*
     * Wir brauchen einen Zeiger auf ein Objekt im Stack-Bereich
     * zum Beispiel auf die Variable i:
     */
    int i;
    int *stack_adresse = &i;

    /**
     * Probiere alle Threads: Wenn die Adresse der lokalen
     * Variablen i innerhalb des Stack-Bereiches einer Thread-
     * Struktur liegt, dann haben wir den Richtigen erwischt.
     */
    for(i = 0; stack_adresse < &threads[i].stack[0]
            || stack_adresse >=  &threads[i].stack[STACKSIZE]; i++)
        if(i == num_threads)
            return((void*)0);  /* nicht gefunden -> Abbruch */

    assert(threads[i].busy);   /* Thread muss aktiv sein, sonst stimmt was nicht */
    return((void*)&threads[i]);
}

/**
 * @brief Thread-Umschaltung
 *
 * Diese Funktion blockiert den Thread des Aufrufers und schaltet zu einem (in der Regel andern)
 * Thread um, der daraufhin aktiv wird. Wird @c handle als @c NULL uebergeben, so wird der naechste
 * Thread anhand einer FIFO-Reihenfolge bestimmt. Falls es nur einen Thread im System gibt, so kann
 * dies auch der Aufrufer selbst sein. Wird mit @c handle der Thread-Handle eines existierenden Thread
 * angegeben, so wird dieser Thread als naechster ausgefuehrt. Falls dieser nicht gueltig existiert, wird
 * wieder in FIFO-Reihenfolge fortgefahren.
 *
 * @param handle Handle des naechsten zu aktivierenden Threads. \b NULL falls das System die Auswahl treffen soll
 * @retval nichts
 */

void uth_thread_next(void *handle)
{
    int i;
    struct thread *selbst;
	struct thread *next;
    /**
     * Was ist zu tun?
     * 1) Den derzeit laufenden Thread bestimmen (mit @c uth_get_handle() )
     * 2) Den naechsten Thread bestimmen: er ist entweder durch @c handle gegeben
     *    (in diesem Fall pruefen, ob @c handle wirklich ein gueltiger Zeiger auf
     *    eine unserer Thread-Strukturen ist). Ansonsten (@c handle = 0) wird der
     *    naechste belegte Thread in der Reihenfolge (zu bestimmen mit
     *    @c next_busy_thread() ) genommen.
     * 3) Mit @c thread_switch() zum naechsten Thread umschalten.
     */

	selbst = (struct thread*)uth_get_handle();
	/* Der aktuelle Thread ist auch der angeforderte Thread */
	if(handle != NULL && handle == selbst) return;
	/* Handle gegeben? */
	if (!handle) {
		next = next_busy_thread(selbst);
		thread_switch(selbst, next);	
	} else {
		thread_switch(selbst, handle);	
	}

}

/**
 * @brief Initialisieren der Micro Thread Bibliothek
 *
 * Diese Funktion initialisiert die Micro Thread Bibliothek und startet
 * den ersten Thread. Dieser kann dann weitere Threads erzeugen und starten.
 * Diese Funktion kehrt erst zurueck, wenn alle Threads der Threadbibliothek beendet sind.
 *
 * @param num Maximale Anzahl an Threads, die erzeugt werden koennen
 * @param entry Eintrittspunkt des ersten Threads
 * @retval nichts
 */
void uth_init(unsigned int num, void (*entry)(void))
{
    /**
     * Was ist zu tun?
     * 1) Reservieren des Speichers fuer die Thread-Strukturen:
     *    Diese Strukturen enthalten unter anderem die Stacks der einzelnen Threads.
     *    Daher sollen sie im Stack-Bereich des UNIX-Programmes, unter dem diese
     *    Bibliothek laeuft, liegen. Wir erreichen das, indem wir hier ein Array
     *    von Thread-Strukturen als lokale Daten der Funktion @c uth_init deklarieren.
     * 2) Damit die anderen Funktionen dieser Bibliothek auch auf diese Thread-Strukturen
     *    zugreifen koennen, definieren wir den globalen Zeiger @c threads auf dieses Array.
     *    dieser wird hier initialisiert.
     * 3) Auch die Anzahl Threads muss als globale Variable @c num_threads bekannt sein,
     *    Diese Variable muss hier zu @c num initialisiert werden.
     * 4) Den ersten Thread erzeugen (mit @c uth_thread_create() )
     * 5) Den ersten Thread direkt mit @c __longjmp() starten, zuvor aber mit @c __setjmp()
     *    den Zustand des UNIX-Prorammes in @c main_context retten, damit wir nach dem
     *    terminieren des letzten Thread hier wieder weiter machen koennen. 
     * 
     */
    struct thread thread_space[num];
    struct thread *init_thread;
    int i;
    /**
     * Damit die anderen Funktionen dieser Bibliothek auch auf diese Thread-Strukturen
     * zugreifen koennen, definieren wir den globalen Zeiger @c threads auf dieses Array.
     * dieser wird hier initialisiert:
     */
    threads = thread_space;

    num_threads = num;
    
    for(i = 0; i < num_threads; i++) {
      threads[i].busy = 0;
    }

    /**
     * num_threads muss mindestens 1 sein, sonst koennen
     * wir noch nicht mal den ersten thread erzeugen!
     */
    if(num_threads < 1)
        return;
    /**
     * Erzeuge den ersten Thread:
     */
    init_thread = (struct thread *)uth_thread_create(entry);
    assert(init_thread);

    /**
     * Rette den aktuellen Kontext, speichere ihn in @c main_context:
     * Wir kehren spaeter (per @c __longjmp() ) hierher zurueck, wenn
     * der letzte Thread beendet ist.
     */
    if(!__setjmp(main_context))
    {   /** springe in den ersten Thread: */
        __longjmp(init_thread->context, 1);
    }
}

