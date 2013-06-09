
create table players (id integer,
                     x float,
                     y float,
                     gold integer
                     );

insert into players values(1, 81.28, 17.29, 3000000);
insert into players values(2, 22.22, 22.22, 500);
insert into players values(3, 232.6, 45.765, 100);

select id, gold from players where (2+2)/2+0.2 == p.id;
