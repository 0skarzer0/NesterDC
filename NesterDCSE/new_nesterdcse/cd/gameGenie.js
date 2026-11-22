
function ggIsSet(aParts)
{
	var aPartsCopy = new Array();
	for (var j = 0; j < aParts.length; ++j)
	{
		var temp = new Object();
		temp.code = aParts[j];
		temp.found = false;
		aPartsCopy[j] = temp;
	}
	for (var k = 0; k < Nester.GameGenie.getCodeCount(); ++k)
	{
		var curSet = Nester.GameGenie.getCode(k);
		for (var n = 0; n < aPartsCopy.length; ++n)
		{
			if (!aPartsCopy[n].found && aPartsCopy[n].code == curSet)
				aPartsCopy[n].found = true;
		}
	}
	
	for (var n = 0; n < aPartsCopy.length; ++n)
	{
		if (!aPartsCopy[n].found)
			return false;
	}
	return true;
}

function xmlFile2menu(filepath, pMenu, built_in, checkIconImg, isVMU)
{
	var thingy = null;
	if (isVMU)
	{
		var f = new VMUFile();
		if (!f.load(filepath))
			return;
		thingy = XML.str2Obj(f.data);
	}
	else
		thingy = XML.file2Obj(filepath);
	if (!thingy)
		return;

	for (var k = 0; k < thingy._children.length; ++k)
	{
		var temp = thingy._children[k];
		if (temp._name == 'code')
		{
			var descNode = null;
			for (var n = 0; n < temp._children.length; ++n)
			{
				if (temp._children[n]._name == 'description')
				{
					descNode = temp._children[n];
					break;
				}
			}
			if (!descNode)
				continue;
			var aCodes = new Array();
			for (var n = 0; n < temp._children.length; ++n)
			{
				if (temp._children[n]._name == 'part')
					aCodes[aCodes.length] = temp._children[n]._text;
			}
			pMenu.items[pMenu.items.length] = {name:descNode._text, parts:aCodes, builtin:built_in};
			if (ggIsSet(aCodes))
			{
				pMenu.items[pMenu.items.length-1].isSet = true;
				pMenu.items[pMenu.items.length-1].iconImg = checkIconImg;
			}
		}
	}
}

function gameGenieScreen()
{
	System.garbageCollect();
	var ggbg = new Image();
	ggbg.load("/cd/graphics/ggbg.jpg", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	
	var checkIconImg = new Image();
	checkIconImg.load("/cd/graphics/check.png", IMG_PUNCH_THRU, IMG_FILTER_NONE, IMG_ALPHA_KEYED);
	
	var codesMenu = new Menu();
	codesMenu.enabled = true;
	codesMenu.iconWidth = checkIconImg.getWidth()+2;
	codesMenu.visibleRowCount = 11;
	codesMenu.items = new Array();
	codesMenu.setColorRule(packColor(225, 0, 225, 0), 'isSet', true);
	
	if (g_loadedGameEntry && g_loadedGameEntry.gamegenie)
		xmlFile2menu(DATA_BASE+"/GameGenie/"+padZeros(g_loadedGameEntry.gamegenie)+".xml", codesMenu, true, checkIconImg, false);
	var vmuPath = findFirstVMU();
	if (vmuPath)
		xmlFile2menu(vmuPath+"/"+Nester.getROMCRCstr()+".gg", codesMenu, false, checkIconImg, true);
	var origCustomCount = 0;
	for (var k = 0; k < codesMenu.items.length; ++k)
	{
		if (!codesMenu.items[k].builtin)
			++origCustomCount;
	}
	
	var actionMenu = new Menu();
	actionMenu.enabled = false;
	actionMenu.visibleRowCount = 11;
	actionMenu.items = new Array(
		{name:"Enter Code", isEnter:true},
		{name:"Clear All", isClear:true}
	);
	
	var menuX = 0;
	var menuY = 0;
	
	var inActions = false;
	if (!g_loadedGameEntry || g_loadedGameEntry.gamegenie == 0)
		inActions = true;
	var inEnter = false;
	var enterStep = 0;
	
	var ggLetters = "AEPOZXLUGKISTVYN";
	var alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890!@#$%^&*()";
	var alphabetSize = alphabet.length;
	
	var curLetterX = 0;
	var curLetterY = 0;
	
	var curCodeEntry = "";
	var codesEntered = new Array();
	var curDesc = "";
	var curAlpha = 0;
	var alphaRepeat = 0;
	
	var badMsg = null;
	var badMsgCount = 0;
	
	var curFrame = 0;
	
	while (true)
	{
		pad1.poll();
		if (!inEnter)
		{
			if (pad1.justPressed("B"))
			{
				var customs = new Array();
				for (var k = 0; k < codesMenu.items.length; ++k)
				{
					if (!codesMenu.items[k].builtin)
						customs[customs.length] = codesMenu.items[k];
				}
				vmuPath = findFirstVMU();
				if (vmuPath)
				{
					var filename = vmuPath+"/"+Nester.getROMCRCstr()+".gg";
					if (customs.length > 0)
					{
						var ggstr = "<gg>";
						for (var k = 0; k < customs.length; ++k)
						{
							var cur = customs[k];
							ggstr += "<code>";
							ggstr += "<description>"+cur.name+"</description>";
							for (var n = 0; n < cur.parts.length; ++n)
								ggstr += "<part>"+cur.parts[n]+"</part>";
							ggstr += "</code>";
						}
						ggstr += "</gg>";

						var f = new VMUFile();
						if (g_loadedGameEntry)
							f.longDesc = g_loadedGameEntry.name;
						else
							f.longDesc = "Some Game";
						f.shortDesc = "Game Genie Codes";
						f.appID = "NesterDC SE";
						f.data = ggstr;
						f.save(filename);
					}
					else if (origCustomCount > 0)
						FileSystem.remove(filename);
				}
				return;
			}
			if (inActions)
			{
				actionMenu.processInput();
				if (pad1.justPressed("A"))
				{
					if (actionMenu.curItem.isClear)
					{
						Nester.GameGenie.removeAllCodes();
						for (var k = 0; k < codesMenu.items.length; ++k)
						{
							codesMenu.items[k].isSet = false;
							codesMenu.curItem.iconImg = null;
						}
					}
					else if (actionMenu.curItem.isEnter)
					{
						curLetterX = 0;
						curLetterY = 0;
						inEnter = true;
						codesMenu.enabled = false;
						actionMenu.enabled = false;
						curCodeEntry = "";
						enterStep = 0;
						codesEntered = new Array();
						curDesc = "";
						curAlpha = 0;
					}
				}
			}
			else
			{
				codesMenu.processInput();
				if (pad1.justPressed("A"))
				{
					if (!codesMenu.curItem.isSet)
					{
						var parts = codesMenu.curItem.parts;
						for (var k = 0; k < parts.length; ++k)
							Nester.GameGenie.addCode(parts[k]);
						codesMenu.curItem.isSet = true;
						codesMenu.curItem.iconImg = checkIconImg;
					}
					else
					{
						var parts = codesMenu.curItem.parts;
						for (var k = 0; k < parts.length; ++k)
							Nester.GameGenie.removeCode(parts[k]);
						codesMenu.curItem.isSet = false;
						codesMenu.curItem.iconImg = null;
					}
				}
				if (pad1.justPressed("Y") && codesMenu.items.length > 0 && !codesMenu.curItem.builtin)
				{
					codesMenu.items.splice(codesMenu.curItemIndex, 1);
					if (codesMenu.curItemIndex == codesMenu.items.length && codesMenu.items.length > 0)
					{
						codesMenu.curItemIndex--;
						if (codesMenu.firstVisibleItem > 0)
							codesMenu.firstVisibleItem--;
					}
					codesMenu.curItem = codesMenu.items[codesMenu.curItemIndex];
				}
			}
			if (pad1.justPressed("LEFT"))
			{
				inActions = false;
				codesMenu.enabled = true;
				actionMenu.enabled = false;
			}
			if (pad1.justPressed("RIGHT"))
			{
				inActions = true;
				codesMenu.enabled = false;
				actionMenu.enabled = true;
			}
		}
		else // inEnter
		{
			if (enterStep == 0)
			{
				if (pad1.justPressed("START") || pad1.justPressed("X"))
				{
					if (curCodeEntry.length == 6 || curCodeEntry.length == 8)
					{
						codesEntered[codesEntered.length] = curCodeEntry;
						curCodeEntry = "";
						if (pad1.justPressed("START"))
							enterStep = 1;
						else
						{
							if (codesEntered.length == 1)
								badMsg = "1 code entered";
							else
								badMsg = codesEntered.length+" codes entered";
						}
					}
					else
						badMsg = "Codes must be 6 or 8\nletters long";
				}
				if (pad1.justPressed("A"))
				{
					if (curCodeEntry.length < 8)
						curCodeEntry += ggLetters.charAt((curLetterY*8)+curLetterX);
				}
				if (pad1.justPressed("B"))
				{
					if (curCodeEntry.length > 0)
						curCodeEntry = curCodeEntry.substr(0, curCodeEntry.length-1);
				}
				if (pad1.justPressed("Y"))
				{
					inEnter = false;
					actionMenu.enabled = true;
				}
				if (pad1.justPressed("LEFT"))
				{
					curLetterX--;
					if (curLetterX < 0)
						curLetterX = 0;
				}
				if (pad1.justPressed("RIGHT"))
				{
					curLetterX++;
					if (curLetterX == 8)
						curLetterX = 7;
				}
				if (pad1.justPressed("UP"))
				{
					curLetterY--;
					if (curLetterY < 0)
						curLetterY = 0;
				}
				if (pad1.justPressed("DOWN"))
				{
					curLetterY++;
					if (curLetterY == 2)
						curLetterY = 1;
				}
			}
			else if (enterStep == 1)
			{
				if (pad1.justPressed("START"))
				{
					if (curDesc.length == 0)
						curDesc = "User Code";
					var userparts = new Array();
					for (var k = 0; k < codesEntered.length; ++k)
						Nester.GameGenie.addCode(codesEntered[k]);
					codesMenu.items[codesMenu.items.length] = {name:curDesc, parts:codesEntered, isSet:true, iconImg:checkIconImg, builtin:false};
					inEnter = false;
					actionMenu.enabled = true;
				}
				if (pad1.justPressed("A"))
				{
					curDesc += alphabet.charAt(curAlpha);
				}
				if (pad1.justPressed("B"))
				{
					if (curDesc.length > 0)
						curDesc = curDesc.substr(0, curDesc.length-1);
				}
				if (pad1.justPressed("Y"))
				{
					inEnter = false;
					actionMenu.enabled = true;
				}
				if (pad1.UP)
				{
					if (pad1.justPressed("UP"))
						alphaRepeat = 0;
					else
						alphaRepeat++;
					if ((alphaRepeat%6)==0)
						curAlpha--;
					if (curAlpha < 0)
						curAlpha = alphabetSize-1;
				}
				if (pad1.DOWN)
				{
					if (pad1.justPressed("UP"))
						alphaRepeat = 0;
					else
						alphaRepeat++;
					if ((alphaRepeat%6)==0)
						curAlpha++;
					if (curAlpha == alphabetSize)
						curAlpha = 0;
				}
			}
		}
		/*
		if (pad1.LEFT)
			menuX--;
		if (pad1.RIGHT)
			menuX++;
		if (pad1.UP)
			menuY--;
		if (pad1.DOWN)
			menuY++;
		println("menuX="+menuX+" menuY="+menuY);
		*/
		
		Video.frameBegin();
		ggbg.draw(0, 0, 1, g_colors["white"], 10);
		codesMenu.draw(16, 18, 50, 326, 288);
		actionMenu.draw(367, 18, 50, 251, 288);
		
		var lettersX = 257;
		var lettersY = 396;
		
		for (var k = 0; k < 2; ++k)
		{
			for (var j = 0; j < 8; ++j)
			{
				main_font.draw(ggLetters.charAt((k*8)+j), lettersX, lettersY, 50, -1, 20, g_colors['white']);
				if (inEnter && enterStep == 0 && j == curLetterX && k == curLetterY)
				{
					var squareX = lettersX-16;
					var squareY = lettersY-31;
					Video.drawRect(squareX, squareY, squareX+47, squareY+45, 80, packColor(128, 0, 255, 0));
				}
				lettersX += 47;
			}
			lettersX = 257;
			lettersY += 45;
		}
		
		if (inEnter)
		{
			var msg = null;
			if (enterStep == 0)
			{
				msg = "Codes Entry\n\nA = Enter Letter\nB = Backspace\nX = Additional Code\nY = Cancel\nStart = Done";
				main_font.draw(curCodeEntry, 241, 334, 50, -1, 20, g_colors['white']);
			}
			else if (enterStep == 1)
			{
				msg = "Description\n\nUp / Down = Change Letter\nA = Enter Letter\nB = Backspace\nY = Cancel\nStart = Done";
				var displayDesc = curDesc;
				if ((curFrame%30) < 25)
					displayDesc += alphabet.charAt(curAlpha);
				main_font.draw(displayDesc, 241, 334, 50, -1, 20, g_colors['white']);
			}
			if (badMsg)
			{
				badMsgCount++;
				if (badMsgCount > 180)
				{
					badMsgCount = 0;
					badMsg = null;
				}
				else
					msg = badMsg;
			}
			
			main_font.draw(msg, 19, 330, 50, -1, 12, g_colors['white']);
		}
		
		Video.frameFinish();
		curFrame++;
	}
}
