create table Person (id integer, name char(20));
insert into Person values (1, 'Dennis Ritchie');
insert into Person values (2, 'Kenneth Lane Thompson');
select name from Person where id = 1;
