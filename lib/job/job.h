#ifndef __INCLUDED_JOB_H_
#define __INCLUDED_JOB_H_

int job_setting(struct charge_task *, struct charge_job *);
int job_wait(struct charge_task *, struct charge_job *);
int job_scanning(struct charge_task *, struct charge_job *);
int job_12v_assit_test(struct charge_task *, struct charge_job *);
int job_24v_assit_test(struct charge_task *, struct charge_job *);
int job_start(struct charge_task *, struct charge_job *);
int job_standby(struct charge_task *, struct charge_job *);
int job_working(struct charge_task *, struct charge_job *);
int job_err_pause(struct charge_task *, struct charge_job *);
int job_man_pause(struct charge_task *, struct charge_job *);
int job_resume(struct charge_task *, struct charge_job *);
int job_abort(struct charge_task *, struct charge_job *);
int job_done(struct charge_task *, struct charge_job *);
int job_exitting(struct charge_task *, struct charge_job *);
int job_detaching(struct charge_task *, struct charge_job *);
int job_dizzy(struct charge_task *, struct charge_job *);

#endif // __INCLUDED_JOB_H_
