
create table nations (id integer,
                      x float,
                      y float
                     );

create table players (id integer,
                      nation_id integer
                     );

create table characters (id integer,
                         player_id integer,
                         x float,
                         y float,
                         gold integer
                        );

insert into nations values(1, 546.0, 32.0);
insert into nations values(2, 1729.0, 8128.0);

insert into players values(1, 2);
insert into players values(2, 1);
insert into players values(3, 2);

insert into characters values(1, 1337, 81.28, 17.29, 3000000);
insert into characters values(2, 4, 22.22, 22.22, 500);
insert into characters values(3, 3, 232.6, 45.765, 100);
insert into characters values(4, 1337, 80.0, 45.765, 4);

select n.id, n.x, n.y
from characters c, players p, nations n
where c.player_id == p.id
  and p.nation_id == n.id
  and c.gold == 100;
