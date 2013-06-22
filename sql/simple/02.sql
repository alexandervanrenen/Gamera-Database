create table Studenten
       (MatrNr         integer,
        Name           char(30) not null,
        Semester       integer);

create table Professoren
       (PersNr         integer,
        Name           char(30) not null,
        Rang           char(2),
        Raum           integer);

create table Assistenten
       (PersNr         integer,
        Name           char(30) not null,
        Fachgebiet     char(30),
        Boss           integer);

create table Vorlesungen
       (VorlNr         integer,
        Titel          char(30),
        SWS            integer,
        gelesenVon     integer);

create table hoeren
       (MatrNr         integer,
        VorlNr         integer);

create table voraussetzen
       (Vorgaenger     integer,
        Nachfolger     integer);

create table pruefen
       (MatrNr         integer,
        VorlNr         integer,
        PersNr         integer,
        Note           float);

insert into Studenten values (24002, 'Xenokrates', 18); 
insert into Studenten values (25403, 'Jonas', 12); 
insert into Studenten values (26120, 'Fichte', 10); 
insert into Studenten values (26830, 'Aristoxenos', 8); 
insert into Studenten values (27550, 'Schopenhauer', 6); 
insert into Studenten values (28106, 'Carnap', 3); 
insert into Studenten values (29120, 'Theophrastos', 2); 
insert into Studenten values (29555, 'Feuerbach', 2); 

insert into Professoren values (2125, 'Sokrates', 'C4', 226);
insert into Professoren values (2126, 'Russel', 'C4', 232); 
insert into Professoren values (2127, 'Kopernikus', 'C3', 310);
insert into Professoren values (2133, 'Popper', 'C3', 052);
insert into Professoren values (2134, 'Augustinus', 'C3', 309);
insert into Professoren values (2136, 'Curie', 'C4', 036);
insert into Professoren values (2137, 'Kant', 'C4', 007);

insert into Assistenten values (3002, 'Platon', 'Ideenlehre', 2125);
insert into Assistenten values (3003, 'Aristoteles', 'Syllogistik', 2125);
insert into Assistenten values (3004, 'Wittgenstein', 'Sprachtheorie', 2126);
insert into Assistenten values (3005, 'Rhetikus', 'Planetenbewegung', 2127);
insert into Assistenten values (3006, 'Newton', 'Keplersche Gesetze', 2127);
insert into Assistenten values (3007, 'Spinoza', 'Gott und Natur', 2134);

insert into Vorlesungen values (5001, 'Grundzuege', 4, 2137);
insert into Vorlesungen values (5041, 'Ethik', 4, 2125);
insert into Vorlesungen values (5043, 'Erkenntnistheorie', 3, 2126);
insert into Vorlesungen values (5049, 'Maeeutik', 2, 2125);
insert into Vorlesungen values (4052, 'Logik', 4, 2125);
insert into Vorlesungen values (5052, 'Wissenschaftstheorie', 3, 2126); 
insert into Vorlesungen values (5216, 'Bioethik', 2, 2126); 
insert into Vorlesungen values (5259, 'Der Wiener Kreis', 2, 2133); 
insert into Vorlesungen values (5022, 'Glaube und Wissen', 2, 2134); 
insert into Vorlesungen values (4630, 'Die 3 Kritiken', 4, 2137); 

insert into hoeren values (26120, 5001);
insert into hoeren values (27550, 5001);
insert into hoeren values (27550, 4052);
insert into hoeren values (28106, 5041);
insert into hoeren values (28106, 5052);
insert into hoeren values (28106, 5216);
insert into hoeren values (28106, 5259);
insert into hoeren values (29120, 5001);
insert into hoeren values (29120, 5041);
insert into hoeren values (29120, 5049);
insert into hoeren values (29555, 5022);
insert into hoeren values (25403, 5022); 
insert into hoeren values (29555, 5001); 

insert into voraussetzen values (5001, 5041);
insert into voraussetzen values (5001, 5043);
insert into voraussetzen values (5001, 5049);
insert into voraussetzen values (5041, 5216);
insert into voraussetzen values (5043, 5052);
insert into voraussetzen values (5041, 5052);
insert into voraussetzen values (5052, 5259);

insert into pruefen values (28106, 5001, 2126, 1.0);
insert into pruefen values (25403, 5041, 2125, 2.0);
insert into pruefen values (27550, 4630, 2137, 2.0);

select s.Name, s.MatrNr
from Studenten s, hoeren h, Vorlesungen v, Professoren p
where s.MatrNr = h.MatrNr
and h.VorlNr = v.VorlNr
and v.gelesenVon = p.PersNr
and p.Name = 'Sokrates';

select s1.Name , s1.MatrNr
from Studenten s1 , Studenten s2 , hoeren h1, hoeren h2
where s1.MatrNr = h1.MatrNr
and s2.MatrNr = h2.MatrNr
and h1.VorlNr = h2.VorlNr
and s2.Name = 'Fichte';
