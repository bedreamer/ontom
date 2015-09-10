// JavaScript Document
// 前一次选中的列表ID 
var pre_li_id = 'id_li_system';
var li_unselect_color = 'rgba(105,105,105,0.5)';
var li_selected_color = 'rgba(0,144,255,0.5)';
var query_str = '';
var edit_id = 'N/A';
var ip='127.0.0.1:8081';
//var ip='192.168.1.58:8081';

function setting_main_loop() {
	var host = document.location.host;
	host = host.replace('8080', '8081');
	ip = host;

	$.getJSON('http://' + ip + '/system/config.json', '', function (data, status, xhr) {
		if ( status == 'success' ) {
			$.each(data, function (index, d) {
				if ( index != 'configs' ) return;
				var codes_system = '';
				var codes_comm = '';
				var codes_charger = '';
				var codes_personal = '';
				var codes_kaichu = '';
				for ( var i = 0; i < d.length; i ++ ) {
					var html = '';
					if ( d[i].type == 'radio' ) {
						html = "<div class=\"option\">";
            			html = html + "<div class=\"option_name\">" + d[i].name + "</div>";
                		html = html + "<div class=\"option_input_panel\">";
						if ( d[i].current_value == d[i].rv_1_value ) {
							var id_first = "id_r_first_" + i.toString();
							var id_second = "id_r_second_" + i.toString();
							html = html + "<a href=\"javascript:on_radio_click('" + id_first;
							html = html + "','" + d[i].key + "','" + d[i].rv_1_value + "');\"><div id=\"" + id_first;
							html = html + "\" class=\"option_radio option_radio_head\">&nbsp;" + d[i].rv_1_name + "&nbsp;</div></a>";
							html = html + "<a href=\"javascript:on_radio_click('" + id_second;
							html = html + "','" + d[i].key + "','" + d[i].rv_2_value +  "');\"><div id=\"" + id_second;
							html = html + "\" class=\"option_radio option_radio_tail\">&nbsp;" + d[i].rv_2_name + "&nbsp;</div></a>";
						} else if ( d[i].current_value == d[i].rv_2_value ) {
							var id_first = "id_r_first_" + i.toString();
							var id_second = "id_r_second_" + i.toString();
							html = html + "<a href=\"javascript:on_radio_click('" + id_second;
							html = html + "','" + d[i].key + "','" + d[i].rv_1_value +  "');\"><div id=\"" + id_second;
							html = html + "\" class=\"option_radio option_radio_tail\">&nbsp;" + d[i].rv_1_name + "&nbsp;</div></a>";
							html = html + "<a href=\"javascript:on_radio_click('" + id_first;
							html = html + "','" + d[i].key + "','" + d[i].rv_2_value +  "');\"><div id=\"" + id_first;
							html = html + "\" class=\"option_radio option_radio_head\">&nbsp;" + d[i].rv_2_name + "&nbsp;</div></a>";
						} else ;
                		html = html + "</div></div>";
					} else if ( d[i].type == 'text' ) {
						html = "<div class=\"option\">";
            			html = html + "<div class=\"option_name\">" + d[i].name + "</div>";
                		html = html + "<div class=\"option_input_panel\">";
						html = html + "<input id=\"id_text_" + i + "\" "; 
						html = html + "onfocus=\"javascript:on_text_focus('id_text_" + i + "');\" "; 
						html = html + "onchange=\"javascript:on_text_changed('id_text_" + i + "','" + d[i].key + "');\" "; 
						html = html + "onblur=\"javascript:on_text_blur('id_text_" + i + "','" + d[i].key + "');\" "; 
						if ( d[i].rv_1_name == 'ip' ) {
							html = html + "class=\"cls_input_box cls_input_ip\" type=\"text\""
						} else {
							html = html + "class=\"cls_input_box\" type=\"text\"";
						}
						html = html + " value=\"" + d[i].current_value;
                		html = html + "\" width=\"" + d[i].rv_1_value + "\" /></div>";
            			html = html + "</div>";
					} else if ( d[i].type == 'options' ) {
						var jstr = d[i].rv_1_name;
						jstr = jstr.replace(/@/g, '"');
						jstr = jstr.replace(/#/g, '{');
						jstr = jstr.replace(/%/g, '}');
						var option = eval(jstr);
						html = "<div class=\"option\">";
						html = html + "<div class=\"option_name\">" + d[i].name + "</div>";
						html = html + "<div class=\"option_input_panel\"><select class=\"select_options\" id=\"id_select_";
						html = html + i + "\" onchange=javascript:on_option_changed('id_select_" + i + "','" + d[i].key + "')>";
						for ( var j = 0; j < option.length; j ++ ) {
							if ( option[j].v == d[i].current_value ) {
								html=html+ "<option selected=\"selected\" value=\"" + option[j].v + "\">" + option[j].n + "</option>";
							} else {
								html = html + "<option value=\"" + option[j].v + "\">" + option[j].n + "</option>";
							}
						}
						html = html + "</select></div></div>";
					} else;
					
					if ( d[i].cat == 'system' ) {
						codes_system = codes_system + html;
					} else if ( d[i].cat == 'comm' ) {
						codes_comm = codes_comm + html;
					} else if ( d[i].cat == 'charger' ) {
						codes_charger = codes_charger + html;
					} else if ( d[i].cat == 'personal' ) {
						codes_personal = codes_personal + html;
					} else if ( d[i].cat == 'kaichu' ) {
						codes_kaichu = codes_kaichu + html;
					}
				}
				$('#id_content_system').html(codes_system);
				$('#id_content_comm').html(codes_comm);
				$('#id_content_charger').html(codes_charger);
				$('#id_content_personal').html(codes_personal);
				$('#id_content_kaichu').html(codes_kaichu);
			});
		}
	});
}

function refresh_query_str(key, value) {
	
	if ( value == '' ) return;
	
	if ( query_str.indexOf(key) >= 0 ) {
		var reg = new RegExp(key + "=[^;]\+");
		var nestr = key + "=" + value;
		query_str = query_str.replace(reg, nestr);
	} else {
		query_str = query_str +  key + "=" + value + ";";
	}
}

function on_radio_click(idd, key, value) {
	var newid_f = '#' + idd;
	var newid_s = '';
	if ( idd.indexOf('id_r_first_') >= 0 ) {
		newid_s = '#' + idd.replace('id_r_first_', 'id_r_second_');
	} else if ( idd.indexOf('id_r_second_') >= 0 ){
		newid_s = '#' + idd.replace('id_r_second_', 'id_r_first_');
	} else ;
	$( newid_f ).css('background-color','rgb(0,144,255)');
	$( newid_s ).css('background-color','#555');
	$( newid_f ).css('color','#FFF');
	$( newid_s ).css('color','#AAA');
	
	refresh_query_str(key, value);
}

function on_option_changed(idd, key) {
	refresh_query_str(key, $('#' + idd).val());
}

function on_text_changed(idd, key ) {
	refresh_query_str(key, $('#' + idd).val());
	edit_id = 'N/A';
}

function on_text_blur(idd, key ) {
	refresh_query_str(key, $('#' + idd).val());
}

$(function(){
	$('#id_li_about').click(function(){
		if ( pre_li_id != 'id_li_about' ) {
			$('#id_li_about').css('background-color', li_selected_color);
			$('#' + pre_li_id).css('background-color', li_unselect_color);
			
			$(pre_li_id.replace('id_li_', '#id_content_')).hide();
			$('#id_content_about').show();
			$('#id_right').hide();
			$('#id_footer').hide();
			$('#id_content').css('width', '75%');
			pre_li_id = 'id_li_about';
		}
	})
});

$(function(){
	$('#id_li_system').click(function(){
		if ( pre_li_id != 'id_li_system' ) {
			$('#id_li_system').css('background-color', li_selected_color);
			$('#' + pre_li_id).css('background-color', li_unselect_color);
			
			$(pre_li_id.replace('id_li_', '#id_content_')).hide();
			$('#id_content_system').show();
			$('#id_right').show();
			$('#id_footer').show();
			$('#id_content').css('width', '50%');
			pre_li_id = 'id_li_system';
		}
	})
});

$(function(){
	$('#id_li_comm').click(function(){
		if ( pre_li_id != 'id_li_comm' ) {
			$('#id_li_comm').css('background-color', li_selected_color);
			$('#' + pre_li_id).css('background-color', li_unselect_color);

			$(pre_li_id.replace('id_li_', '#id_content_')).hide();
			$('#id_content_comm').show();
			$('#id_right').show();
			$('#id_footer').show();
			$('#id_content').css('width', '50%');
			pre_li_id = 'id_li_comm';
		}
	})
});

$(function(){
	$('#id_li_charger').click(function(){
		if ( pre_li_id != 'id_li_charger' ) {
			$('#id_li_charger').css('background-color', li_selected_color);
			$('#' + pre_li_id).css('background-color', li_unselect_color);
			
			$(pre_li_id.replace('id_li_', '#id_content_')).hide();
			$('#id_content_charger').show();
			$('#id_right').show();
			$('#id_footer').show();
			$('#id_content').css('width', '50%');
			pre_li_id = 'id_li_charger';
		}
	})
});

$(function(){
	$('#id_li_personal').click(function(){
		if ( pre_li_id != 'id_li_personal' ) {
			$('#id_li_personal').css('background-color', li_selected_color);
			$('#' + pre_li_id).css('background-color', li_unselect_color);
			
			$(pre_li_id.replace('id_li_', '#id_content_')).hide();
			$('#id_content_personal').show();
			$('#id_right').show();
			$('#id_footer').show();
			$('#id_content').css('width', '50%');
			pre_li_id = 'id_li_personal';
		}
	})
});

$(function(){
	$('#id_li_kaichu').click(function(){
		if ( pre_li_id != 'id_li_kaichu' ) {
			$('#id_li_kaichu').css('background-color', li_selected_color);
			$('#' + pre_li_id).css('background-color', li_unselect_color);
			
			$(pre_li_id.replace('id_li_', '#id_content_')).hide();
			$('#id_content_kaichu').show();
			$('#id_right').show();
			$('#id_footer').show();
			$('#id_content').css('width', '50%');
			pre_li_id = 'id_li_kaichu';
		}
	})
});

function on_text_focus(id) {
	edit_id = id;
}

function return_main_page() {
	window.location.href = '/index.html'
}

var t = 0;
function refresh_notify_text() {
	var str;
	var tips = ['.', '..', '...', '....', '.....'];
	
	if ( t > 4 ) {
		t = 0;
	}

	str = '正在保存' + tips[ t ];
	t ++;

	$('#id_content_text').html(str);
	setTimeout(refresh_notify_text, 300);
	setTimeout(return_main_page, 1500);
}

var ut = 0;
var bt = 0;
function refresh_upgrade_notify_text() {
	var str;
	var tips = ['.', '..', '...', '....', '.....'];

	if ( ut > 4 ) {
		ut = 0;
	}

	if ( bt < 100 ) bt = bt + 1;
	
	str = '正在升级' + tips[ ut ] + '(' + bt + '%)';
	ut ++;

	$('#id_upgrade_text').html(str);
	setTimeout(refresh_upgrade_notify_text, 300);
	if ( bt >= 100 ) {
		setTimeout(return_main_page, 10);
	}
}

function key_push(key) {
	if ( key == 'r' ) {
		setting_main_loop();
	} else if ( key == 's' ) {
		$('#id_select_box').show();
	} else if ( key == 'S' ) {
		t = 0;
		if ( query_str.length > 0 ) {
			setTimeout(refresh_notify_text, 10);
			$.getJSON('http://' + ip + '/system/save.json', 's='+query_str);
			query_str = '';
		}
	} else if ( key == 'C' ) {
		$('#id_select_box').hide();
	} else {
		if ( edit_id == 'N/A' ) return;
		$('#' + edit_id ).focus();
		if ( key == 'c' ) {
			$('#' + edit_id ).val('');
		} else {
			var v = $('#' + edit_id ).val();
			$('#' + edit_id ).val( v.toString() + key );
		}
	}
}

// 检查更新
$(function(){
	$('#update').click(function(){
		$('#id_upgrade_box').show();
		setTimeout(refresh_upgrade_notify_text, 10);
		$.getJSON('http://' + ip + '/system/update.json', '', '');
	})
});

// 导出日志 
$(function(){
	$('#export').click(function(){
		setTimeout(refresh_notify_text, 10);
		$.getJSON('http://' + ip + '/system/export.json', '', '');
	})
});
