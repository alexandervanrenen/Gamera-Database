create table Persons (id integer, firstname char(20), lastname char(20), job char(20));

insert into Persons values(0, 'Lenard', 'Hofstadter', 'Physics');
insert into Persons values(1, 'Penny', 'no name', 'Waitress');
insert into Persons values(2, 'Sheldon', 'Cooper', 'Physics');
insert into Persons values(3, 'Rajesh', 'koothrappali', 'Astrophysics');
insert into Persons values(4, 'Howard', 'Wolowitz', 'Engineer');

create table Relations (type char(20), lhs integer, rhs integer);

insert into Relations values('best friends', 0, 2);
insert into Relations values('best friends', 4, 3);
insert into Relations values('sex', 1, 0);

select p1.firstname
from Persons p1
where p1.id = 2;
