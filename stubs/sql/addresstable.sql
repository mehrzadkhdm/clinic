-- Table: address

-- DROP TABLE address;

CREATE TABLE address
(
  id bigserial NOT NULL,
  version integer NOT NULL,
  created_at timestamp without time zone,
  updated_at timestamp without time zone,
  line1 text NOT NULL,
  zip text NOT NULL,
  zip text NOT NULL,
  local_id text NOT NULL,
  address_id text NOT NULL,
  CONSTRAINT address_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE address
  OWNER TO clinicore;
