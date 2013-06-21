create table Persons (firstname char(20), lastname char(20), job char(20));

insert into Persons values('Lenard', 'Hofstadter', 'Physics');
insert into Persons values('Penny', 'no name', 'Waitress');
insert into Persons values('Sheldon', 'Cooper', 'Physics');
insert into Persons values('Rajesh', 'koothrappali', 'Astrophysics');
insert into Persons values('Howard', 'Wolowitz', 'Engineer');

create table Relations (type char(20), lhs char(20), rhs char(20));

insert into Relations values('best friends', 'Lenard', 'Sheldon');
insert into Relations values('best friends', 'Howard', 'Rajesh');
insert into Relations values('sex', 'Penny', 'Lenard');
