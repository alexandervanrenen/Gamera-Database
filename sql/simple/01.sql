create table studenten (id integer, semester integer, name char(20), nachname char(20), age integer, unique(age), unique(id,age));

insert into studenten values(1, 7, 'alex', 'van renen', 24);
insert into studenten values(5, 6, 'benjamin', 'schwarz', 24);
insert into studenten values(3, 6, 'seb', 'woehr', 24);
insert into studenten values(2, 6, 'cons', 'gers', 24);

select id, name
from studenten
where id < 5
order by name, id;
