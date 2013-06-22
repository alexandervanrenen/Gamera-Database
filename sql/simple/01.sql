create table studenten (id integer, semester integer, name char(20), nachname char(20), age integer);

insert into studenten values(1, 7, 'alex', 'van renen', 24);
insert into studenten values(2, 6, 'benjamin', 'schwarz', 24);

select name, nachname
from studenten
where id = 1;
