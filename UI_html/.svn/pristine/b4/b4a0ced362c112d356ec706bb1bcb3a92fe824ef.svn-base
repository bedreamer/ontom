// 获得焦点的编辑框ID
var editid = 0;
var value = "";

// 只针对输入编辑框, 对于多个编辑框时有效
function vk_focurs(id)
{
	if ( id != editid && id != null  ) {
		value = document.getElementById(id).value;
		editid = id;
		value = "";
	}
	editid = id;
	//document.getElementById('id_keypad').style.display = 'block';
}

function vk_pushed(vkey)
{
	switch ( vkey ) {
		case 'dot':
			value = value.toString() + ".";
		break;
		case 'del':
			value = "";
		break;
		case 0:case 1:case 2:case 3:case 4:
		case 5:case 6:case 7:case 8:case 9:
			value = value.toString() + vkey.toString();
		break;
	}
	if ( editid != 0 ) {
		document.getElementById(editid).value = value;
	}
}
