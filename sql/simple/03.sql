
create table players (id integer,
                     x float,
                     y float,
                     gold integer
                     );

insert into players values(1, 81.28, 17.29, 3000000);
insert into players values(2, 21.28, 32.29, 2);

select s.id, gold from players s;
