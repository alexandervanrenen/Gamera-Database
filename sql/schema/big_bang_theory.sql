create table Persons (id integer, firstname char(20), lastname char(20), job char(20));

insert into Persons values(0, 'Lenard', 'Hofstadter', 'Physics');
insert into Persons values(1, 'Penny', 'no name', 'Waitress');
insert into Persons values(2, 'Sheldon', 'Cooper', 'Physics');
insert into Persons values(3, 'Rajesh', 'koothrappali', 'Astrophysics');
insert into Persons values(4, 'Howard', 'Wolowitz', 'Engineer');

create table Relations (type char(20), lhs char(20), rhs char(20));

insert into Relations values('best friends', 'Lenard', 'Sheldon');
insert into Relations values('best friends', 'Howard', 'Rajesh');
insert into Relations values('sex', 'Penny', 'Lenard');

select firstname from Persons where false = true;
