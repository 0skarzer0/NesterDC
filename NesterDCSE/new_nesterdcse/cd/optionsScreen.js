
var optTabDist = 350;

function optionsScreen()
{
	System.garbageCollect();
	var optionsbg = new Image();
	optionsbg.load("/cd/graphics/optionsbg.jpg", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	var optionsMenu = new Menu();
	optionsMenu.fontSize = 30;
	optionsMenu.visibleRowCount = 12;
	optionsMenu.items = new Array(
		{name : "Input", func : selectControllerOptions},
		{name : "Video", func : videoOptions},
		{name : "Lists", func : listOptions},
		{name : "View README", func : viewReadme}
	);
	while (true)
	{
		pad1.poll();
		optionsMenu.processInput();
		if (pad1.justPressed("A"))
			optionsMenu.curItem.func(optionsbg);
		else if (pad1.justPressed("B") || pad1.justPressed("Y"))
		{
			saveSettings();
			return mainMenu;
		}
		Video.frameBegin();
		optionsbg.draw(0, 0, 1, g_colors["white"], 10);
		optionsMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function viewReadme(bgImage)
{
	System.garbageCollect();
	var fp = new File();
	if (fp.open("/cd/readme.txt", "rb"))
	{
		viewText(fp.readAll(), bgImage, 65, false);
		fp.close();
	}
}

function videoOptions(bgImage)
{
	var optionsMenu = new Menu();
	optionsMenu.fontSize = 30;
	optionsMenu.visibleRowCount = 12;
	optionsMenu.items = new Array(
		{name:"Fullscreen Stretch[TAB]"+optTabDist+"[/TAB]"+(g_settings.renderTrue?"No":"Yes"), isRenderTrue:true},
		{name:"Bilinear Filtering[TAB]"+optTabDist+"[/TAB]"+(Nester.bilinearFiltering?"Yes":"No"), isBilinear:true},
		{name:"Show FPS[TAB]"+optTabDist+"[/TAB]"+(g_settings.showFPS?"Yes":"No"), isShowFPS:true},
		{name:"NES Type[TAB]"+optTabDist+"[/TAB]"+(g_settings.nesModeNTSC?"NTSC":"PAL"), isNESMode:true},
		{name:"Adjust Screen", func:adjustScreen}
	);
	while (true)
	{
		pad1.poll();
		optionsMenu.processInput();
		if (pad1.justPressed("A"))
		{
			if (optionsMenu.curItem.func)
				optionsMenu.curItem.func(bgImage);
		}
		if (optionsMenu.curItem.isBilinear && (pad1.justPressed("LEFT") || pad1.justPressed("RIGHT")))
		{
			Nester.bilinearFiltering = !Nester.bilinearFiltering;
			optionsMenu.curItem.name = "Bilinear Filtering[TAB]"+optTabDist+"[/TAB]"+(Nester.bilinearFiltering?"On":"Off");
		}
		if (optionsMenu.curItem.isShowFPS && (pad1.justPressed("LEFT") || pad1.justPressed("RIGHT")))
		{
			g_settings.showFPS = !g_settings.showFPS;
			optionsMenu.curItem.name = "Show FPS[TAB]"+optTabDist+"[/TAB]"+(g_settings.showFPS?"Yes":"No");
		}
		if (optionsMenu.curItem.isNESMode && (pad1.justPressed("LEFT") || pad1.justPressed("RIGHT")))
		{
			g_settings.nesModeNTSC = !g_settings.nesModeNTSC;
			optionsMenu.curItem.name = "NES Type[TAB]"+optTabDist+"[/TAB]"+(g_settings.nesModeNTSC?"NTSC":"PAL");
		}
		if (optionsMenu.curItem.isRenderTrue && (pad1.justPressed("LEFT") || pad1.justPressed("RIGHT")))
		{
			g_settings.renderTrue = !g_settings.renderTrue;
			optionsMenu.curItem.name = "Fullscreen Stretch[TAB]"+optTabDist+"[/TAB]"+(g_settings.renderTrue?"No":"Yes");
		}
		if (pad1.justPressed("B") || pad1.justPressed("Y"))
			return mainMenu;
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		Nester.crosshairImg.draw(100, 300, 1, g_colors["white"], 10);
		optionsMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function doAddList(bgImage)
{
	//var test = new TestPoint(107, 300);
	var str = "";
	var oskb = new OSKeyboard();
	while (true)
	{
		pad1.poll();
		if (pad1.justPressed("B"))
			return null;
		var c = oskb.processInput();
		
		if (c == "DONE")
			return str;
		else if (c !== null)
		{
			if (c == "BS")
				str = str.substring(0, str.length-1);
			else
				str += c;
		}
		
		//test.processInput();
		
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		oskb.draw(82, 270);
		var tempHalfWidth = main_font.getWidth(str, 40) / 2;
		main_font.draw(str, 320-tempHalfWidth, 160, 300, -1, 40, g_colors["white"]);
		Video.frameFinish();
	}
}

function doDeleteList(bgImage)
{
	//var test = new TestPoint(107, 300);
	
	var optionsMenu = new Menu();
	optionsMenu.fontSize = 30;
	optionsMenu.visibleRowCount = 12;
	optionsMenu.items = new Array();
	for (var k in g_settings)
	{
		if (typeof(k) == "string" && k.indexOf("list_") == 0)
			optionsMenu.items[optionsMenu.items.length] = {name:k.substr(5), varName:k};
	}
	
	while (true)
	{
		pad1.poll();
		optionsMenu.processInput();
		if (pad1.justPressed("B"))
			return;
		else if (pad1.justPressed("A"))
		{
			delete g_settings[optionsMenu.curItem.varName];
			return;
		}
		
		//test.processInput();
		
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		optionsMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function listOptions(bgImage)
{
	var optionsMenu = new Menu();
	optionsMenu.fontSize = 30;
	optionsMenu.visibleRowCount = 12;
	optionsMenu.items = new Array();
	optionsMenu.items[optionsMenu.items.length] = {name:"Add List", isAdd:true};
	optionsMenu.items[optionsMenu.items.length] = {name:"Delete List", isDelete:true};
	while (true)
	{
		pad1.poll();
		optionsMenu.processInput();
		if (pad1.justPressed("A"))
		{
			if (optionsMenu.curItem.isAdd)
			{
				var newListName = doAddList(bgImage);
				if (newListName)
					g_settings["list_"+newListName] = new Array();
			}
			else if (optionsMenu.curItem.isDelete)
				doDeleteList(bgImage);
			continue;
		}
		if (pad1.justPressed("B") || pad1.justPressed("Y"))
			return mainMenu;
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		optionsMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function miscOptions(bgImage)
{
	var optionsMenu = new Menu();
	optionsMenu.fontSize = 30;
	optionsMenu.visibleRowCount = 12;
	optionsMenu.items = new Array(
		{name:"Browsing Type[TAB]"+optTabDist+"[/TAB]"+(g_settings.romBrowseMode=='SE'?"SE":"Disc"), isBrowsingMode:true}
	);
	while (true)
	{
		pad1.poll();
		optionsMenu.processInput();
		if (optionsMenu.curItem.isBrowsingMode && (pad1.justPressed("LEFT") || pad1.justPressed("RIGHT")))
		{
			g_settings.romBrowseMode = (g_settings.romBrowseMode=='SE')?'FILE':'SE';
			optionsMenu.curItem.name = "Browsing Type[TAB]"+optTabDist+"[/TAB]"+(g_settings.romBrowseMode=='SE'?"SE":"Disc");
		}
		if (pad1.justPressed("B") || pad1.justPressed("Y"))
			return mainMenu;
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		optionsMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function selectControllerOptions(bgImage)
{
	System.garbageCollect();
	var selectMenu = new Menu();
	selectMenu.fontSize = 30;
	selectMenu.visibleRowCount = 12;
	selectMenu.items = new Array(
		{name:"Player 1", index:0},
		{name:"Player 2", index:1},
		{name:"Player 3", index:2},
		{name:"Player 4", index:3}
	);
	while (true)
	{
		pad1.poll();
		selectMenu.processInput();
		if (pad1.justPressed("A"))
			controllerOptions(bgImage, selectMenu.curItem.index);
		else if (pad1.justPressed("B") || pad1.justPressed("Y"))
			return;
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		selectMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function NESButtonToText(val)
{
	switch (val)
	{
		case NES_A:
			return "A";
		case NES_B:
			return "B";
		case NES_SELECT:
			return "SELECT";
		case NES_START:
			return "START";
		case NES_COIN:
			return "COIN";
		case NES_A_AUTOFIRE:
			return "AUTOFIRE A";
		case NES_B_AUTOFIRE:
			return "AUTOFIRE B";
		case NES_QUICK_SAVE:
			return "QUICK SAVE";
		case NES_QUICK_LOAD:
			return "QUICK LOAD";
	};
	return "HUH?!";
}

function DCButtonToText(val)
{
	var result = "";
	while (true)
	{
		if (val & CONT_X)
		{
			if (result.length > 0)
				result += " + ";
			result += "X";
			val &= ~CONT_X;
		}
		else if (val & CONT_Y)
		{
			if (result.length > 0)
				result += " + ";
			result += "Y";
			val &= ~CONT_Y;
		}
		else if (val & CONT_A)
		{
			if (result.length > 0)
				result += " + ";
			result += "A";
			val &= ~CONT_A;
		}
		else if (val & CONT_B)
		{
			if (result.length > 0)
				result += " + ";
			result += "B";
			val &= ~CONT_B;
		}
		else if (val & CONT_START)
		{
			if (result.length > 0)
				result += " + ";
			result += "START";
			val &= ~CONT_START;
		}
		else if (val & CONT_DPAD_UP)
		{
			if (result.length > 0)
				result += " + ";
			result += "UP";
			val &= ~CONT_DPAD_UP;
		}
		else if (val & CONT_DPAD_DOWN)
		{
			if (result.length > 0)
				result += " + ";
			result += "DOWN";
			val &= ~CONT_DPAD_DOWN;
		}
		else if (val & CONT_DPAD_LEFT)
		{
			if (result.length > 0)
				result += " + ";
			result += "LEFT";
			val &= ~CONT_DPAD_LEFT;
		}
		else if (val & CONT_DPAD_RIGHT)
		{
			if (result.length > 0)
				result += " + ";
			result += "RIGHT";
			val &= ~CONT_DPAD_RIGHT;
		}
		else if (val & CONT_L)
		{
			if (result.length > 0)
				result += " + ";
			result += "L";
			val &= ~CONT_L;
		}
		else if (val & CONT_R)
		{
			if (result.length > 0)
				result += " + ";
			result += "R";
			val &= ~CONT_R;
		}
		else if (val & EMPTY_MAPPING)
		{
			if (result.length > 0)
				result += " + ";
			result += "(NONE)";
			val &= ~EMPTY_MAPPING;
		}
		else
			break;
	}
	return result;
}

function autoFirespeedToText(speed)
{
	switch (speed)
	{
		case AUTOFIRE_SLOW:
			return "Slow";
		case AUTOFIRE_MEDIUM:
			return "Medium";
		case AUTOFIRE_FAST:
			return "Fast";
		case AUTOFIRE_INSANE:
			return "Insane";
	};
	return "WHAT?!";
}

function setControllerMenuItems(pMenu, whichController)
{
	pMenu.items = new Array();
	
	pMenu.items[pMenu.items.length] = { name:"Use Analog[TAB]"+optTabDist+"[/TAB]"+(Nester.getUseAnalog(whichController)?"Yes":"No"), isAnalog:true };
	pMenu.items[pMenu.items.length] = { name:"Auto-Fire A Speed[TAB]"+optTabDist+"[/TAB]"+autoFirespeedToText(Nester.getAutofireSpeed(whichController, NES_A)), isAutofire:true, nesButton:NES_A, buttonName:"A" };
	pMenu.items[pMenu.items.length] = { name:"Auto-Fire B Speed[TAB]"+optTabDist+"[/TAB]"+autoFirespeedToText(Nester.getAutofireSpeed(whichController, NES_B)), isAutofire:true, nesButton:NES_B, buttonName:"B" };
	pMenu.items[pMenu.items.length] = BLANK_LINE;
	for (var k = 0; k < NES_COUNT; ++k)
	{
		var tempstr = NESButtonToText(k) + "[TAB]"+optTabDist+"[/TAB]" + DCButtonToText(Nester.getButtonMapping(whichController, k));
		pMenu.items[pMenu.items.length] = { name:tempstr, button:k, isMapping:true };
	}
}

function controllerOptions(bgImage, whichController)
{
	System.garbageCollect();
	var mapMenu = new Menu();
	mapMenu.fontSize = 30;
	mapMenu.visibleRowCount = 12;
	setControllerMenuItems(mapMenu, whichController);
	while (true)
	{
		pad1.poll();
		mapMenu.processInput();
		if (pad1.justPressed("A"))
		{
			if (mapMenu.curItem.isMapping)
			{
				controllerSetButton(bgImage, whichController, mapMenu.curItem.button);
				setControllerMenuItems(mapMenu, whichController);
			}
		}
		if (pad1.justPressed("LEFT"))
		{
			if (mapMenu.curItem.isAnalog)
			{
				var b = !Nester.getUseAnalog(whichController);
				Nester.setUseAnalog(whichController, b);
				mapMenu.curItem.name = "Use Analog[TAB]"+optTabDist+"[/TAB]"+(b?"Yes":"No");
			}
			else if (mapMenu.curItem.isAutofire)
			{
				var newSpeed = null;
				switch (Nester.getAutofireSpeed(whichController, mapMenu.curItem.nesButton))
				{
					case AUTOFIRE_SLOW:
						newSpeed = AUTOFIRE_INSANE;
						break;
					case AUTOFIRE_MEDIUM:
						newSpeed = AUTOFIRE_SLOW;
						break;
					case AUTOFIRE_FAST:
						newSpeed = AUTOFIRE_MEDIUM;
						break;
					case AUTOFIRE_INSANE:
						newSpeed = AUTOFIRE_FAST;
						break;
				}
				Nester.setAutofireSpeed(whichController, mapMenu.curItem.nesButton, newSpeed);
				mapMenu.curItem.name = "Auto-Fire "+mapMenu.curItem.buttonName+" Speed[TAB]"+optTabDist+"[/TAB]"+autoFirespeedToText(newSpeed);
			}
		}
		if (pad1.justPressed("RIGHT"))
		{
			if (mapMenu.curItem.isAnalog)
			{
				var b = !Nester.getUseAnalog(whichController);
				Nester.setUseAnalog(whichController, b);
				mapMenu.curItem.name = "Use Analog[TAB]"+optTabDist+"[/TAB]"+(b?"Yes":"No");
			}
			else if (mapMenu.curItem.isAutofire)
			{
				var newSpeed = null;
				switch (Nester.getAutofireSpeed(whichController, mapMenu.curItem.nesButton))
				{
					case AUTOFIRE_SLOW:
						newSpeed = AUTOFIRE_MEDIUM;
						break;
					case AUTOFIRE_MEDIUM:
						newSpeed = AUTOFIRE_FAST;
						break;
					case AUTOFIRE_FAST:
						newSpeed = AUTOFIRE_INSANE;
						break;
					case AUTOFIRE_INSANE:
						newSpeed = AUTOFIRE_SLOW;
						break;
				}
				Nester.setAutofireSpeed(whichController, mapMenu.curItem.nesButton, newSpeed);
				mapMenu.curItem.name = "Auto-Fire "+mapMenu.curItem.buttonName+" Speed[TAB]"+optTabDist+"[/TAB]"+autoFirespeedToText(newSpeed);
			}
			
		}
		else if (pad1.justPressed("B") || pad1.justPressed("Y"))
			return;
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		mapMenu.draw(16,16,50,622,461);
		Video.frameFinish();
	}
}

function controllerSetButton(bgImage, whichController, whichButton)
{
	var frameCount = 0;
	var justIn = true;
	var value = 0;
	while (true)
	{
		pad1.poll();
		if (justIn && pad1.A)
			continue;
		justIn = false;
		if (pad1.A && !(value & CONT_A))
		{
			value |= CONT_A;
			frameCount = 0;
		}
		if (pad1.B && !(value & CONT_B))
		{
			value |= CONT_B;
			frameCount = 0;
		}
		if (pad1.X && !(value & CONT_X))
		{
			value |= CONT_X;
			frameCount = 0;
		}
		if (pad1.Y && !(value & CONT_Y))
		{
			value |= CONT_Y;
			frameCount = 0;
		}
		if (pad1.START && !(value & CONT_START))
		{
			value |= CONT_START;
			frameCount = 0;
		}
		if (pad1.UP && !(value & CONT_DPAD_UP))
		{
			value |= CONT_DPAD_UP;
			frameCount = 0;
		}
		if (pad1.DOWN && !(value & CONT_DPAD_DOWN))
		{
			value |= CONT_DPAD_DOWN;
			frameCount = 0;
		}
		if (pad1.LEFT && !(value & CONT_DPAD_LEFT))
		{
			value |= CONT_DPAD_LEFT;
			frameCount = 0;
		}
		if (pad1.RIGHT && !(value & CONT_DPAD_RIGHT))
		{
			value |= CONT_DPAD_RIGHT;
			frameCount = 0;
		}
		if (pad1.LTRIG > 128 && !(value & CONT_L))
		{
			value |= CONT_L;
			frameCount = 0;
		}
		if (pad1.RTRIG > 128 && !(value & CONT_R))
		{
			value |= CONT_R;
			frameCount = 0;
		}
		Video.frameBegin();
		bgImage.draw(0, 0, 1, g_colors["white"], 10);
		var str = NESButtonToText(whichButton) + " = " + DCButtonToText(value);
		main_font.draw(str, 320-(main_font.getWidth(str, 30)/2), 240, 100, -1, 30, g_colors["white"]);
		Video.frameFinish();
		
		frameCount++;
		if (frameCount == 120)
			break;
	}
	Nester.setButtonMapping(whichController, whichButton, value);
}

function adjustScreen(bgImage)
{
	var fWidth = Video.width;
	var fHeight = Video.height;
	
	var messages = new Array(
		"Adjust the screen position and size",
		"using the D-Pad and analog stick.",
		"START sets default.",
		"Press A when finished."
		);
	
	while (true)
	{
		pad1.poll();
		if (pad1.justPressed("A") || pad1.justPressed("B"))
			break;
		else if (pad1.justPressed("START"))
		{
			Video.x = 0;
			Video.y = 0;
			Video.width = fWidth = 640;
			Video.height = fHeight = 480;
		}
		if (pad1.LEFT)
			Video.x--;
		if (pad1.RIGHT)
			Video.x++;
		if (pad1.UP)
			Video.y--;
		if (pad1.DOWN)
			Video.y++;
		fWidth += pad1.XAXIS / 127;
		fHeight += pad1.YAXIS / 127;
		Video.width = fWidth;
		Video.height = fHeight;

		Video.frameBegin();
		if (bgImage)
			bgImage.draw(0, 0, 1, g_colors["white"], 10);
		else
			Nester.draw();
		var cury = 200;
		for (var k = 0; k < messages.length; ++k)
		{
			main_font.draw(messages[k], 320-(main_font.getWidth(messages[k], 30) / 2), cury, 100, -1, 30, g_colors["white"]);
			cury += 40;
		}
		Video.frameFinish();
	}
}