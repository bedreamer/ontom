CREATE TABLE log(
	timestamp string,
	logstring string
	);
CREATE TABLE errors(
	error_seq_id unsigned int,
	error_id string,
	error_begin string,
	error_recover string,
	recover_reason string,
	error_string string
	);