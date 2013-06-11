
create table nations (id integer,
                      name char(20),
                      x float,
                      y float
                     );

create table players (id integer,
                      name char(20),
                      nation_id integer
                     );

create table characters (id integer,
                         name char(20),
                         player_id integer,
                         x float,
                         y float,
                         gold integer
                        );

insert into nations values(1, "Germany", 546.0, 32.0);
insert into nations values(2, "France", 1729.0, 8128.0);

insert into players values(1, "alex", 2);
insert into players values(2, "sebastian", 2);
insert into players values(3, "constantin", 1);

insert into characters values(1, "flowly", 1, 81.28, 17.29, 3000000);
insert into characters values(2, "sebseb", 2, 22.22, 22.22, 500);
insert into characters values(3, "conscons", 3, 232.6, 45.765, 100);
insert into characters values(1, "zweigchen", 1, 80.0, 45.765, 4);

select n.id, n.name
from characters c, players p, nations n
where c.player_id = p.id
  and p.nation_id = n.id
  and c.name = "flowly";
