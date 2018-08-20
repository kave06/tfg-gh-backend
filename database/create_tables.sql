create table devices
(
	id int auto_increment,
	mac varchar(17) default '' not null,
	zone varchar(100) null,
	model varchar(15) null,
	primary key (id, mac)
)
;

create table ambient_data
(
	sensor int default '0' not null,
	date datetime default '0000-00-00 00:00:00' not null,
	temp float null,
	humi float null,
	primary key (sensor, date),
	constraint ambient_data_ibfk_1
		foreign key (sensor) references devices (id)
)
;

create table irrigation
(
	id int auto_increment
		primary key,
	id_relay int not null,
	start datetime not null,
	end datetime not null,
	liters int null,
	constraint `irrigation_devices(id)__fk`
		foreign key (id_relay) references devices (id)
)
;

create view sensor1_per_hour as select `prototipo`.`ambient_data`.`sensor`           AS `sensor`,
         `prototipo`.`ambient_data`.`temp`             AS `temp`,
         `prototipo`.`ambient_data`.`humi`             AS `humi`,
         `prototipo`.`ambient_data`.`date`             AS `date`,
         year(`prototipo`.`ambient_data`.`date`)       AS `year`,
         month(`prototipo`.`ambient_data`.`date`)      AS `month`,
         dayofmonth(`prototipo`.`ambient_data`.`date`) AS `day`,
         hour(`prototipo`.`ambient_data`.`date`)       AS `hour`
  from `prototipo`.`ambient_data`
  where (`prototipo`.`ambient_data`.`sensor` = 1)
  group by year(`prototipo`.`ambient_data`.`date`), month(`prototipo`.`ambient_data`.`date`),
           dayofmonth(`prototipo`.`ambient_data`.`date`), hour(`prototipo`.`ambient_data`.`date`)
  order by `prototipo`.`ambient_data`.`date` desc
;

create view sensor2_per_hour as select `prototipo`.`ambient_data`.`sensor`           AS `sensor`,
         `prototipo`.`ambient_data`.`temp`             AS `temp`,
         `prototipo`.`ambient_data`.`humi`             AS `humi`,
         `prototipo`.`ambient_data`.`date`             AS `date`,
         year(`prototipo`.`ambient_data`.`date`)       AS `year`,
         month(`prototipo`.`ambient_data`.`date`)      AS `month`,
         dayofmonth(`prototipo`.`ambient_data`.`date`) AS `day`,
         hour(`prototipo`.`ambient_data`.`date`)       AS `hour`
  from `prototipo`.`ambient_data`
  where (`prototipo`.`ambient_data`.`sensor` = 2)
  group by year(`prototipo`.`ambient_data`.`date`), month(`prototipo`.`ambient_data`.`date`),
           dayofmonth(`prototipo`.`ambient_data`.`date`), hour(`prototipo`.`ambient_data`.`date`)
  order by `prototipo`.`ambient_data`.`date` desc
;

create view sensor3_per_hour as select `prototipo`.`ambient_data`.`sensor`           AS `sensor`,
         `prototipo`.`ambient_data`.`temp`             AS `temp`,
         `prototipo`.`ambient_data`.`humi`             AS `humi`,
         `prototipo`.`ambient_data`.`date`             AS `date`,
         year(`prototipo`.`ambient_data`.`date`)       AS `year`,
         month(`prototipo`.`ambient_data`.`date`)      AS `month`,
         dayofmonth(`prototipo`.`ambient_data`.`date`) AS `day`,
         hour(`prototipo`.`ambient_data`.`date`)       AS `hour`
  from `prototipo`.`ambient_data`
  where (`prototipo`.`ambient_data`.`sensor` = 3)
  group by year(`prototipo`.`ambient_data`.`date`), month(`prototipo`.`ambient_data`.`date`),
           dayofmonth(`prototipo`.`ambient_data`.`date`), hour(`prototipo`.`ambient_data`.`date`)
  order by `prototipo`.`ambient_data`.`date` desc
;
