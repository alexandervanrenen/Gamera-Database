   
External Sort
=============

Wir haben uns entschieden einen Sort-Merge Alogrithmus zu implementieren.

* Die Source Dateien liegen in src/external_sort/*
* Die Test Dateien findest du in test/external_sort/*

Die Funktion:
-------------
"void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize)"
ist in src/external_sort/ExternalSortInterface.hpp definiert. Diese wandelt die parameter um und ruft die eigentliche Sortier-Funktion auf, welche sich in src/external_sort/ExternalSort.* befindet.

Der Algorithmus teilt sich in 2 Phasen auf. Als erstes erstellen wir die Runs, dann mergen wir sie. Ein paar Hinweise zum code:

* Die InputRun Klasse kapselt das lesen eines Runs weg.
* Die OutputRun Klasse kapselt das schrieben eines Runs weg.
* Der FileNameProvider erstellt unique file names.
* Der MemoryManager verwaltet den zur verfügung gestellten speicher.
* Wir benutzen den size parameter nicht. Es wird also immer das ganze File sortiert.
* Den code in der ExternalSort::mergeRunPhase ist eventuell etwas verwirrent. Hier werden die Runs für die nächste Merge-Phase ausgewählt.
* Für den k-way-merge haben wurde einen std::multiset (tree) genommen. Es wurde auch ein LoserTree (;)) implementiert aber noch nicht in den Sort-Algorithmus integriert.
* Bei Fragen email an alex.

Compile:
--------

"make sort" erstellt das gewünschte command line Tool. Wir haben die Speichergröße ein wenig beschränkt. Bitte nicht unter ca 1024 Byte benutzen .. Da geht sicher was kaputt.

Unsere tests compilieren mit: "make uebung1" und sind sond dann mit ./uebung1 auszuführen. Der 5 GB Testfall ist auch da drin, aber auskommentiert, weil ich meine SSD nicht umbringen will jedesmal wenn ich teste. (Einfach der unterste Testfall in test/external_sort/ExternalSortTest.hpp).

Buffer Manager
==============

Der Buffer Manager sorgt dafür das teile einer großen Datei in einen verhältnissmässig kleinen Arbeitsspeicher gemappt werden.

Features:
---------

Optimiert auf wenige Locks.

* Ein fix auf eine schon im Speicher vorhandene Seite benötigt nur ein R/W Lock auf den Frame.
* Ein fix auf eine noch nicht im Speicher vorhandene Seite beötigt einen globalen Lock (für die Datei), welcher nur während dem schreiben und lesen gehalten wird. Dazu wird der Frame lokal mit einem Write-Lock geocket und die Hashmap wird bei der Veränderung mit einem lokalen Lock blockiert. Also keine globalen Locks ausser auf dem File, welcher unvermeidlich ist, wenn wir nur eine Datei haben.
* Ein unfix benötigt keinen Lock.

Optimierte Implementierungen für kritische System teile:

* Hash map mit fixierter Größe und Offsets.
* Read Write lock für downgrates von Locks.
* SpinLock wrapper.

Austauschbare Algorithmen zum Verdrängen von Seiten. Im BufferManager.hpp den Typen ändern.

* Radom
* Second chance
* Two Queue

Issues:
-------

Einige Probleme in der Implementierung:

* Valgrind scheint mit dem SpinLock im BufferManager nicht klar zu kommen, dieser kann über die using Anweisung: "using LockType = util::SpinLock;" im BufferManager Header File geändert werden. (Einfach auf eine std::mutex setzen, falls du es valgrinden möchtest).
* Kein guter Test für die Verdrängungs Algorithmen, da es schwer ist das Verhalten einer Datenbank zu simulieren.

Segment Manager
===============

The segment manager creates Segments upon pages. One Segment is the SPSegment, which serves as a primary means for storing data in out database system. Some general information:

* The SegmentIventory stores the mapping from SegmentId to its Extents for every Segment in a persistent way. (physical)
* The SegmentManger uses the SegmentInventory to create Segment objects from this mapping. (logical)
* There is exactly one FreeSpaceInventory which keeps track of the free bytes of each page in the system.

Segment Inventory
-----------------

* Uses a Persister object to store the SegmentId to Extents mapping on disc. The Persister maintains a linked list of Pages containing the mapping data. Whenever a new Segment is created a new entry is inserted. A update will trigger an update on the page and a drop segment will remove the mapping entry. This approach allows for constant time updates.
* When restarting the database the Persister crawls through the linked list of meta pages and restores the SegmentId to Extent mapping. Knowing which segments are used we can easily construct the free pages. Our global free space inventory is treated as a normal segment and therefore loaded afterwards.

Slotted Pages
-------------

* Uses slots to keep track of used records. A slot consists of a 16 bit offset and 16 bit length. The most significant bit in both values is used to encode the type of record: Normal Record, Redirected to another page record (called reference), Redirected from another page record (called foreigner) and a unused memory slot.

Features
--------

* Slotted Pages allow for slot reuse and defragmentation.
* SPSegment guarantees that an update is always possible by allowing for overflow records with a maximum chain length of one.
* Extensive testing.
