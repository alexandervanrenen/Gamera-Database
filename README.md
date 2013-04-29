
External Sort
=============

Wir haben uns entschieden einen Sort-Merge Alogrithmus zu implementieren.

- Die Source Dateien liegen in src/external_sort/*
- Die Test Dateien findest du in test/external_sort/*

Exercise 1:
-----------

Die Funktion:
"void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize)"
ist in src/external_sort/ExternalSortInterface.hpp definiert. Diese wandelt die parameter um und ruft die eigentliche Sortier-Funktion auf, welche sich in src/external_sort/ExternalSort.* befindet.

Der Algorithmus teilt sich in 2 Phasen auf. Als erstes erstellen wir die Runs, dann mergen wir sie. Ein paar Hinweise zum code:

- Die InputRun Klasse kapselt das lesen eines Runs weg.
- Die OutputRun Klasse kapselt das schrieben eines Runs weg.
- Der FileNameProvider erstellt unique file names.
- Der MemoryManager verwaltet den zur verfügung gestellten speicher.
- Wir benutzen den size parameter nicht. Es wird also immer das ganze File sortiert.
- Den code in der ExternalSort::mergeRunPhase ist eventuell etwas verwirrent. Hier werden die Runs für die nächste Merge-Phase ausgewählt.
- Für den k-way-merge haben wurde einen std::multiset (tree) genommen. Es wurde auch ein LoserTree (;)) implementiert aber noch nicht in den Sort-Algorithmus integriert.
- Bei Fragen email an alex.

Exercise 2:
-----------

"make sort" erstellt das gewünschte command line Tool. Wir haben die Speichergröße ein wenig beschränkt. Bitte nicht unter ca 1024 Byte benutzen .. Da geht sicher was kaputt.

Other:
------

Unsere tests compilieren mit: "make uebung1" und sind sond dann mit ./uebung1 auszuführen. Der 5 GB Testfall ist auch da drin, aber auskommentiert, weil ich meine SSD nicht umbringen will jedesmal wenn ich teste. (Einfach der unterste Testfall in test/external_sort/ExternalSortTest.hpp).
