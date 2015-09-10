// JavaScript Document

var jiaozhun=[
	{n:'V1',p:4000},
	{n:'V1',p:4500},
	{n:'V1',p:5000},
	{n:'V1',p:5500},
	{n:'V1',p:6000},
	{n:'V1',p:6500},
	{n:'V2',p:6500},
	{n:'V2',p:6000},
	{n:'V2',p:5500},
	{n:'V2',p:5000},
	{n:'V2',p:4500},
	{n:'V2',p:4000},
	{n:'done',p:4000}
];

var jiaozhun_low=[
	{n:'V1',p:2000},
	{n:'V1',p:2500},
	{n:'V1',p:3000},
	{n:'V1',p:3500},
	{n:'V1',p:4000},
	{n:'V1',p:4500},
	{n:'V1',p:5000},
	{n:'V2',p:5000},
	{n:'V2',p:4500},
	{n:'V2',p:4000},
	{n:'V2',p:3500},
	{n:'V2',p:3000},
	{n:'V2',p:2500},
	{n:'V2',p:2000},

	{n:'done',p:2000}
];

var ceshidian=[
	{n:'CV1',p:4250},
	{n:'CV1',p:4750},
	{n:'CV1',p:5250},
	{n:'CV1',p:5750},
	{n:'CV1',p:6250},
	{n:'CV1',p:6400},
	{n:'CV2',p:6400},
	{n:'CV2',p:6250},
	{n:'CV2',p:5750},
	{n:'CV2',p:5250},
	{n:'CV2',p:4750},
	{n:'CV2',p:4250},
	{n:'CV2',p:4000},
	{n:'done',p:4000}
];
var started = false;
var cur = 0;
var done = false;
var confirm = 0;
var saved = false;
var jingdu = 5;
var jiaozhunfanwei = 'high_lev';

function jiaozhun_proc() {
	if ( jiaozhun[cur].n == 'V1' ) {
		$('#id_name').html('一段母线及电池1电压');
	} else if ( jiaozhun[cur].n == 'V2' ) {
		$('#id_name').html('二段母线及电池2电压');
	} else if ( jiaozhun[cur].n == 'I' ) {
		$('#id_name').html('电池电流');
	}
	$('#id_param').html(jiaozhun[cur].p/10 + 'V');
	$('#id_ps').html( '第' + (cur * 10 + confirm + 1) + '项 / 共' + (jiaozhun.length * 10 - 10 )+ '项' );
	var host = document.location.host;
	host = host.replace('8080', '8081');

	if (jiaozhun.length > cur) {

		var p = 'op=' + jiaozhun[cur].n + '&p=' + jiaozhun[cur].p;
		if ( jiaozhun[cur].n == 'done' ) {
			cur = 0;
			done = true;
			$('#id_param').html('完成');
			return;
		}		
		$.getJSON('http://' + host + '/system/jiaozhun.json', p, function(data, status, xhr){
			if ( status != 'success' ) return;
			if ( jiaozhun[cur].n == 'V1' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'V');
				if ( data.V1 >= (jiaozhun[cur].p/10 - (jiaozhun[cur].p/10)*jingdu/1000) &&
					 data.V1 <= (jiaozhun[cur].p/10 + (jiaozhun[cur].p/10)*jingdu/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			} else if ( jiaozhun[cur].n == 'V2' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'V');
				if ( data.V2 >= (jiaozhun[cur].p/10 - (jiaozhun[cur].p/10)*jingdu/1000) &&  
					 data.V2 <= (jiaozhun[cur].p/10 + (jiaozhun[cur].p/10)*jingdu/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			} else if ( jiaozhun[cur].n == 'I' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'I');
				if ( data.V2 >= (jiaozhun[cur].p/10 - (jiaozhun[cur].p/10)*jingdu/1000) &&  
					 data.V2 <= (jiaozhun[cur].p/10 + (jiaozhun[cur].p/10)*jingdu/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			}


			if ( data.SCS == 'ERR' || data.CCS == 'ERR' ) {
				$('#id_status').html('通信故障');
			} else {
				$('#id_status').html('正常');
			} 
		});
	}
	if ( done == false ) {
		setTimeout(jiaozhun_proc, 1000);
	}
}

function jiaozhun_low_proc() {
	if ( jiaozhun_low[cur].n == 'V1' ) {
		$('#id_name').html('一段母线及电池1电压');
	} else if ( jiaozhun_low[cur].n == 'V2' ) {
		$('#id_name').html('二段母线及电池2电压');
	} else if ( jiaozhun_low[cur].n == 'I' ) {
		$('#id_name').html('电池电流');
	}
	$('#id_param').html(jiaozhun_low[cur].p/10 + 'V');
	$('#id_ps').html( '第' + (cur * 10 + confirm + 1) + '项 / 共' + (jiaozhun_low.length * 10 - 10 )+ '项' );
	var host = document.location.host;
	host = host.replace('8080', '8081');

	if (jiaozhun_low.length > cur) {

		var p = 'op=' + jiaozhun_low[cur].n + '&p=' + jiaozhun_low[cur].p;
		if ( jiaozhun_low[cur].n == 'done' ) {
			cur = 0;
			done = true;
			$('#id_param').html('完成');
			return;
		}		
		$.getJSON('http://' + host + '/system/jiaozhun.json', p, function(data, status, xhr){
			if ( status != 'success' ) return;
			if ( jiaozhun_low[cur].n == 'V1' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'V');
				if ( data.V1 >= (jiaozhun_low[cur].p/10 - (jiaozhun_low[cur].p/10)*jingdu/1000) &&
					 data.V1 <= (jiaozhun_low[cur].p/10 + (jiaozhun_low[cur].p/10)*jingdu/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			} else if ( jiaozhun_low[cur].n == 'V2' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'V');
				if ( data.V2 >= (jiaozhun_low[cur].p/10 - (jiaozhun_low[cur].p/10)*jingdu/1000) &&  
					 data.V2 <= (jiaozhun_low[cur].p/10 + (jiaozhun_low[cur].p/10)*jingdu/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			} else if ( jiaozhun_low[cur].n == 'I' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'I');
				if ( data.V2 >= (jiaozhun_low[cur].p/10 - (jiaozhun_low[cur].p/10)*jingdu/1000) &&  
					 data.V2 <= (jiaozhun_low[cur].p/10 + (jiaozhun_low[cur].p/10)*jingdu/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			}


			if ( data.SCS == 'ERR' || data.CCS == 'ERR' ) {
				$('#id_status').html('通信故障');
			} else {
				$('#id_status').html('正常');
			} 
		});
	}
	if ( done == false ) {
		setTimeout(jiaozhun_low_proc, 1000);
	}
}

$(function(){
	$('#id_jingdu_sub').click(function(){
		jingdu ++;
		$('#id_jingdu').html(jingdu + "/1000");
	})
});

$(function(){
	$('#id_jingdu_add').click(function(){
		jingdu --;
		$('#id_jingdu').html(jingdu + "/1000");
	})
});

$(function(){
	$('#id_start').click(function(){
		if ( started == false ) {
			started = true;
			cur = 0;
			done = false;
			$('#id_btn_start').html('停止校准');
			if (jiaozhunfanwei == 'high_lev') {
				setTimeout(jiaozhun_proc, 50);
			} else {
				setTimeout(jiaozhun_low_proc, 50);
			}
		} else {
			started = false;
			cur = 0;
			done = true;
			$('#id_btn_start').html('开始自动校准');
		}
	})
});

$(function(){
	$('#id_jiaozhunfanwei').click(function(){
		if ( jiaozhunfanwei == 'high_lev' ) {
			jiaozhunfanwei = 'low_lev';
			alert("切换至 200V ~ 450V 校准");
		} else {
			jiaozhunfanwei = 'high_lev';
			alert("切换至 400V ~ 650V 校准");
		}
	})
});



function ceshi_proc() {
	if ( ceshidian[cur].n == 'CV1' ) {
	$('#id_name').html('测试 一段母线及电池1 采样电压');
	} else if ( ceshidian[cur].n == 'CV2' ) {
		$('#id_name').html('测试 二段母线及电池2 采样电压');
	}
	$('#id_param').html(ceshidian[cur].p/10 + 'V');
	$('#id_ps').html( '第' + (cur * 10 + confirm + 1) + '项 / 共' + (ceshidian.length * 10 - 10) + '项' );
	var host = document.location.host;
	host = host.replace('8080', '8081');

	if (ceshidian.length > cur) {
		var p = 'op=' + ceshidian[cur].n + '&p=' + ceshidian[cur].p;
		if ( jiaozhun[cur].n == 'done' ) {
			cur = 0;
			done = true;
			$('#id_param').html('完成');
			$.getJSON('http://' + host + '/system/jiaozhun.json', 'op=done&p=4000', function(data, status, xhr){});
			return;
		}	
		$.getJSON('http://' + host + '/system/jiaozhun.json', p, function(data, status, xhr){
			if ( status != 'success' ) return;
			if ( ceshidian[cur].n == 'CV1' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'V');
				if ( data.V1 >= (ceshidian[cur].p/10 - (ceshidian[cur].p/10)*5/1000) &&
					 data.V1 <= (ceshidian[cur].p/10 + (ceshidian[cur].p/10)*5/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			} else if ( ceshidian[cur].n == 'CV2' ) {
				$('#id_param').html($('#id_param').html()+'/'+data.V1 + 'V');
				if ( data.V2 >= (ceshidian[cur].p/10 - (ceshidian[cur].p/10)*5/1000) &&  
					 data.V2 <= (ceshidian[cur].p/10 + (ceshidian[cur].p/10)*5/1000) ) {
					confirm ++;
				} else {
					confirm = 0;
				}
				if ( confirm >= 10 ) {
					cur ++;
				}
			}

			if ( data.SCS == 'ERR' || data.CCS == 'ERR' ) {
				$('#id_status').html('通信故障');
			} else {
				$('#id_status').html('正常');
			} 
		});
	}
	if ( done == false ) {
		setTimeout(ceshi_proc, 1000);
	}
}

$(function(){
	$('#id_btn_test').click(function(){
		
		if ( done == false ) {
			setTimeout(ceshi_proc, 500);
		}

	})
});

function do_result_save() {
	var host = document.location.host;
	host = host.replace('8080', '8081');
	$.getJSON('http://' + host + '/system/jiaozhun.json', 'op=done&p=4000', function(data, status, xhr){
		if ( status != 'success' ) return;
		if ( data.saved != 'yes' ) {
			saved = false;
			setTimeout(do_result_save, 500);
		} else {
			saved = true;

			$.getJSON('http://' + host + '/system/save.json', 's=work_mode=normal;', function(data, status, xhr){
			});
			window.location = 'http://' + document.location.host + '/index.html'
		}
	});
}

$(function(){
	$('#id_done').click(function(){
		do_result_save();
	})
});
