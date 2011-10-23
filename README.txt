Paper zum Vortrag.

Mikrokontroller haben meist keine Fließkommaberechnungs-Hardware. Will
man jedoch Flißkommazahlen berechnen, so wird eine sehr
rechenaufwendige Software Funktionen automatisch durch den Kompiler
hinzugelinkt. Viele Mikrokontroller haben auch keine Hardware zum
dividieren und multiplizieren von Ganzzahlen.
Mit Hilfe von Ganzzahl-Arithmetik und weiteren Optimierungen kann der
Mikrokontroller wesentlich schneller komplexe Rechnungen durchführen.
Durch den Farbborg sowie in meinem Beruf habe ich viel Erfahrung mit
Optimierungen gesammelt und möchte in diesem Vortrag einigen bewährte
Vorgehensweisen, Tricks und Beispielquellcode zeigen. Im Anschluß an
den  Vortragsteil wird es bei bedarf einen Workshop-Teil geben, in dem
die Teilnehmer Aufgaben mit einem C-Kompiler auf dem PC lösen. Für den
Workshop sind Programmierkenntnis in der Sprache C erfoderlich.
Benötigt wird ein gcc.

Ich werde im Groben folgende Themenbereiche behandeln:

 - Zahlensysteme im Rechner. Signed oder unsigened, char, short oder long.
 - Schieben statt Teilen, Konstanten vorberechnen.
 - Genauigkeit durch die Reihenfolge der Berechnung erhöhen.
 - 1 ist jetzt 1024 und schon hat man 3 Nachkommastellen. Was ist zu beachten.
 - Berechnungen umstellen. Nichtveränderliche Werte vorberechnen.
 - Rechnen mit dem Präprozessor.
 - Trigonometrische Funktionen sowie Wurzeln.
 - Das Problem reduzieren z.B. durch Lookuptables.
 - printf zu langsam? Was macht man alternativ.
 - Digitalen PI-Regler mit Ganzzahlen.
 - Genauigkeitsuntersuchungen mit Hilfe von Unittests.
 