
includeOnce("/cd/colors.js");
includeOnce("/cd/menu.js");
includeOnce("/cd/ramcache.js");
includeOnce("/cd/titleScreen.js");
includeOnce("/cd/optionsScreen.js");
includeOnce("/cd/text_viewer.js");
includeOnce("/cd/gameGenie.js");
includeOnce("/cd/nsfPlayer.js");
includeOnce("/cd/testpoint.js");
includeOnce("/cd/oskeyboard.js");

// Default settings
var g_settings = new Object();
g_settings.romBrowseMode = 'SE';
g_settings.normalBrowseDir = '/cd';
g_settings.showFPS = false;
g_settings.renderTrue = true;
g_settings.nesModeNTSC = !Video.isPAL;
g_settings.list_Favorites = new Array();

//var DATA_BASE = '/cd/library';
var DATA_BASE = '/pc/home/Administrator/projects/new_nesterdcse/builder/test';

var main_font = new Font();
main_font.load("/cd/fonts/helvetica_bold.txf");

Nester.crosshairImg = new Image();
var curCross = "cross.png";
Nester.crosshairImg.load("/cd/crosshairs/cross.png", IMG_PUNCH_THRU, IMG_FILTER_NONE, IMG_ALPHA_KEYED);

var g_gameDir = DATA_BASE+"/roms";
var g_extraNames = new Array('screenshot', 'titleshot', 'cartscan', 'boxscan');
var g_extraLongNames = new Array('In-Game', 'Title', 'Cartridge', 'Box');

var g_extraExts = new Object();
g_extraExts.screenshot = ".png";
g_extraExts.titleshot = ".png";
g_extraExts.cartscan = ".jpg";
g_extraExts.boxscan = ".jpg";

var g_extraDirs = new Object();
g_extraDirs.screenshot = DATA_BASE+"/ScreenShots/";
g_extraDirs.titleshot = DATA_BASE+"/TitleShots/";
g_extraDirs.cartscan = DATA_BASE+"/CartScans/";
g_extraDirs.boxscan = DATA_BASE+"/BoxScans/";
g_extraDirs.faqs = DATA_BASE+"/FAQs/";

var g_nesDB = new NESDB();
var g_romFilename = null;
var romBrowser = null;
var musicInitted = false;
var songPlaying = false;
var musicFiles = null;
var pad1 = Input.controller[0];
var selectedIcon = 0;
var directoryCache = new Object();
var g_loadedGameEntry = null;
var dcfps = Video.isPAL?50:60;

function findFirstVMU()
{
	var vmuList = FileSystem.getFileList('/vmu');
	if (vmuList.length > 0)
		return vmuList[0].fullPath;
	return null;
}

function saveSettings()
{
	var vmuPath = findFirstVMU();
	if (!vmuPath)
		return;
	
	var str = '';
	for (var k in g_settings)
	{
		if (typeof(k) == "string" && k.indexOf("list_") == 0)
		{
			str += 'var L=g_settings["'+k+'"]=new Array();\n';
			for (var n = 0; n < g_settings[k].length; ++n)
				str += 'L['+n+']='+g_settings[k][n]+';\n';
		}
		else if (typeof(g_settings[k]) == "boolean" || typeof(g_settings[k]) == "number")
			str += 'g_settings.'+k+'='+g_settings[k]+';\n';
		else
			str += 'g_settings.'+k+'="'+g_settings[k]+'";\n';
	}
	for (var k = 0; k < 4; ++k)
	{
		for (var n = 0; n < NES_COUNT; ++n)
			str += 'Nester.setButtonMapping('+k+', '+n+', '+Nester.getButtonMapping(k, n)+');\n';
		str += 'Nester.setUseAnalog('+k+', '+(Nester.getUseAnalog(k)?'true':'false')+');\n';
		str += 'Nester.setAutofireSpeed('+k+', NES_A, '+Nester.getAutofireSpeed(k, NES_A)+');\n';
		str += 'Nester.setAutofireSpeed('+k+', NES_B, '+Nester.getAutofireSpeed(k, NES_B)+');\n';
	}
	str += 'Video.x='+Video.x+'\n';
	str += 'Video.y='+Video.y+'\n';
	str += 'Video.width='+Video.width+'\n';
	str += 'Video.height='+Video.height+'\n';
	//print('\n'+str+'\n');
	var f = new VMUFile();
	f.longDesc = "NesterDC SE User Settings";
	f.appID = "NesterDC SE";
	f.data = str;
	f.save(vmuPath+'/ndcse.cfg');
}

function loadSettings()
{
	var vmuPath = findFirstVMU();
	if (!vmuPath)
		return;
	var f = new VMUFile();
	if (f.load(vmuPath+'/ndcse.cfg'))
	{
		//println(f.data);
		eval(f.data);
	}
}

function isInList(listName, dbIndex)
{
	for (var k = 0; k < g_settings[listName].length; ++k)
	{
		if (g_settings[listName][k] == dbIndex)
			return true;
	}
	return false;
}

function addToList(listName, dbIndex)
{
	if (isInList(listName, dbIndex))
		return;
	g_settings[listName][g_settings[listName].length] = dbIndex;
	g_settings[listName].sort(function(a, b)
	{
		return a-b;
	});
	saveSettings();
}

function removeFromList(listName, dbIndex)
{
	for (var k = 0; k < g_settings[listName].length; ++k)
	{
		if (g_settings[listName][k] == dbIndex)
		{
			g_settings[listName].splice(k, 1);
			saveSettings();
			return;
		}
	}
}

var g_viewImageDirReturnStep = null;
var g_viewDir = null;

function viewImageDir()
{
	System.garbageCollect();
	var files = FileSystem.getFileList(g_viewDir);
	
	if (files.length == 0)
		return g_viewImageDirReturnStep;
	
	Video.frameBegin();
	main_font.draw("Loading...", 50, 50, 100, -1, 30, g_colors["white"]);
	Video.frameFinish();
	
	var curimg = new Image();
	if (!curimg.load(files[0].fullPath, IMG_OPAQUE, IMG_FILTER_BILINEAR, IMG_ALPHA_NONE))
		return g_viewImageDirReturnStep;
	
	var i = 0;
	var x = 320;
	var y = 240;
	var scale = 1;
	var angle = 0;
	var showName = true;
	
	while (true)
	{
		pad1.poll();
		if (pad1.justPressed("B"))
			break;
		var oldi = i;
		if (pad1.justPressed("RIGHT") && i < (files.length-1))
			i++;
		if (pad1.justPressed("LEFT") && i > 0)
			i--;
		if (pad1.justPressed("Y"))
			showName = !showName;
		if (i != oldi)
		{
			Video.frameBegin();
			main_font.drawWithBorder("Loading...", 50, 50, 100, -1, 30, g_colors["white"], g_colors["black"]);
			curimg.drawRotated(x, y, scale, g_colors["white"], 15, 0.5, 0.5, angle);
			Video.frameFinish();
			curimg.unload();
			curimg.load(files[i].fullPath, IMG_OPAQUE, IMG_FILTER_BILINEAR, IMG_ALPHA_NONE);
		}
		if (pad1.LTRIG > 0)
			scale -= (pad1.LTRIG / (256*60))*scale;
		if (pad1.RTRIG > 0)
			scale += (pad1.RTRIG / (256*60))*scale;
		x -= (pad1.XAXIS / 32)*scale;
		y -= (pad1.YAXIS / 32)*scale;
		if (pad1.UP)
			angle -= 0.05;
		if (pad1.DOWN)
			angle += 0.05;
		if (pad1.justPressed("X"))
			angle = 0;
		
		Video.frameBegin();
		if (showName)
			main_font.drawWithBorder(files[i].name, 50, 50, 100, -1, 30, g_colors["white"], g_colors["black"]);
		curimg.drawRotated(x, y, scale, g_colors["white"], 15, 0.5, 0.5, angle);
		Video.frameFinish();
	}
	
	return g_viewImageDirReturnStep;
}

function padZeros(thing)
{
	var temp = new String(thing);
	for (var k = temp.length; k < 4; ++k)
		temp = "0"+temp;
	return temp;
}

var ssCounter = 16;

function screenshot()
{
	Video.takeScreenshot('/pc/home/Administrator/NesterDCSE'+padZeros(ssCounter++)+'.ppm');
}

function loadupGameData(recIndex, whichImg, noImage)
{
	if (!whichImg)
		whichImg = 'screenshot';
	if (!noImage)
		noImage = false;
	var dbEntry = g_nesDB.get(recIndex);
	if (!noImage && dbEntry[whichImg] != 0)
	{
		dbEntry.shotImg = new Image();
		dbEntry.shotImg.load(g_extraDirs[whichImg]+padZeros(dbEntry[whichImg])+g_extraExts[whichImg], IMG_OPAQUE, IMG_FILTER_BILINEAR, IMG_ALPHA_NONE);
	}
	else
		dbEntry.shotImg = null;
	dbEntry.halfWidth = Math.floor(main_font.getWidth(dbEntry.name, testsize) / 2);
	return dbEntry;
}

var localIndex = 0;
var testsize = 20;
var curExtra = 1;
var curListName = null;

function getDBRec(i)
{
	if (curListName)
		return g_settings[curListName][i];
	else
		return i;
}

function getDBSize()
{
	if (curListName)
		return g_settings[curListName].length;
	else
		return g_nesDB.size();
}

function activateNextList()
{
	var useNext = curListName == null;
	for (var k in g_settings)
	{
		if (typeof(k) == "string" && k.indexOf("list_") == 0)
		{
			if (k == curListName)
				useNext = true;
			else if (useNext)
			{
				curListName = k;
				return;
			}
		}
	}
	curListName = null;
}

function whichFAQToLoad(bgimg, dbEntry, imgx, imgy, namex, namey, imgscale, menux, menuy)
{
					
	var faqMenu = new Menu();
	faqMenu.visibleRowCount = 18;
	faqMenu.items = new Array();
	
	var zf = new ZipFile();
	zf.open(DATA_BASE+"/FAQs/"+padZeros(dbEntry.faqs)+".zip");
	zf.goToFirstFile();
	var fileinfo = null;
	while (true)
	{
		var fileinfo = zf.getCurrentFileInfo();
		faqMenu.items[faqMenu.items.length] = {name:fileinfo.filename};
		if (!zf.goToNextFile())
			break;
	}
	
	while (true)
	{
		pad1.poll();
		faqMenu.processInput();

		if (pad1.justPressed("A"))
		{
			zf.goToFirstFile();
			while (true)
			{
				var fileinfo = zf.getCurrentFileInfo();
				if (faqMenu.curItem.name == fileinfo.filename)
				{
					var str = zf.readCurrentFileIntoString();
					viewText(str, bgimg, 65);
					System.garbageCollect();
					break;
				}
				if (!zf.goToNextFile())
					break;
			}
		}
		else if (pad1.justPressed("B"))
		{
			zf.close();
			return;
		}

		Video.frameBegin();
		faqMenu.draw(menux,menuy,50,321,436);
		bgimg.draw(0, 0, 1, g_colors["white"], 10);
		if (dbEntry.shotImg)
		{
			dbEntry.shotImg.draw(imgx, imgy, imgscale, g_colors["white"], 100+(imgscale*100));
			main_font.draw(dbEntry.name, namex, namey, 300, -1, 25, g_colors["white"]);
		}
		Video.frameFinish();
	}
}

function whatToLoadMenu(bgimg, dbEntry, imgx, imgy, namex, namey, imgscale)
{
	if (dbEntry.shotImg)
	{
		imgx -= dbEntry.shotImg.getWidth() / 2;
		imgy -= dbEntry.shotImg.getHeight() / 2;
	}
	else
	{
		imgx = 0;
		imgy = 0;
	}
	
	var orig_imgx = imgx;
	var orig_imgy = imgy;
	var orig_textsize = testsize;
	var orig_namex = namex;
	var orig_namey = namey;
	
	var actionMenu = new Menu();
	actionMenu.visibleRowCount = 18;
	actionMenu.items = new Array();
	actionMenu.items[actionMenu.items.length] = {name:"Play Game", isPlay:true};
	for (var k in g_settings)
	{
		if (typeof(k) == "string" && k.indexOf("list_") == 0)
		{
			var curIsInList = isInList(k, getDBRec(localIndex));
			var displayName = k.substr(5);
			actionMenu.items[actionMenu.items.length] = {name:(curIsInList?"Remove From "+displayName:"Add To "+displayName), isList:true, listName:k, inList:curIsInList};
		}
	}
	if (dbEntry.manual)
		actionMenu.items[actionMenu.items.length] = {name:"View Manual", isManual:true};
	if (dbEntry.faqs)
		actionMenu.items[actionMenu.items.length] = {name:"View FAQs", isFAQs:true};
	if (dbEntry.nsf)
		actionMenu.items[actionMenu.items.length] = {name:"Listen to NSF", isNSF:true};
	
	var curtextsize = testsize;
	
	while (true)
	{
		var curstep = 0;
		var stepcount = 30;

		var textstep = (25 - curtextsize) / stepcount;

		var stepx = (25 - imgx) / stepcount;
		var stepy = (71 - imgy) / stepcount;
		var stepscale = (1 - imgscale) / stepcount;

		var menux = 640;
		var menuy = 70;
		var menuxstep = ((25 + (dbEntry.shotImg ? dbEntry.shotImg.getWidth() : 0) + 20) - menux) / stepcount;

		var namestepx = (25 - namex) / stepcount;
		var namestepy = (51 - namey) / stepcount;

		while (true)
		{
			pad1.poll();
			actionMenu.processInput();

			if (pad1.justPressed("A"))
			{
				if (actionMenu.curItem.isPlay)
				{
					stopNSF();
					g_romFilename = g_gameDir+"/"+padZeros(dbEntry.romnum)+".zip";
					println("loading: "+g_romFilename);
					Nester.loadROM(g_romFilename);
					if (dbEntry.shotImg)
						dbEntry.shotImg.unload();
					g_loadedGameEntry = dbEntry;
					return runGame;
				}
				if (actionMenu.curItem.isList)
				{
					if (actionMenu.curItem.inList)
					{
						removeFromList(actionMenu.curItem.listName, getDBRec(localIndex));
						actionMenu.curItem.name = "Add To "+actionMenu.curItem.listName.substr(5);
					}
					else
					{
						addToList(actionMenu.curItem.listName, getDBRec(localIndex));
						actionMenu.curItem.name = "Remove From "+actionMenu.curItem.listName.substr(5);
					}
					actionMenu.curItem.inList = !actionMenu.curItem.inList;
				}
				else if (actionMenu.curItem.isNSF)
				{
					stopNSF();
					g_playNSFReturnStep = loadGameScreenNew;
					g_romFilename = DATA_BASE+"/nsf/"+padZeros(dbEntry.nsf)+".zip";
					println(g_romFilename);
					Nester.loadROM(g_romFilename);
					if (dbEntry.shotImg)
					{
						dbEntry.shotImg.unload();
						dbEntry.shotImg = null;
					}
					g_nsf_dbEntry = dbEntry;
					return nsfPlayer;
				}
				else if (actionMenu.curItem.isManual)
				{
					g_viewImageDirReturnStep = loadGameScreenNew;
					g_viewDir = DATA_BASE+"/Manuals/"+padZeros(dbEntry.manual)+"/";
					return viewImageDir;
				}
				else if (actionMenu.curItem.isFAQs)
				{
					whichFAQToLoad(bgimg, dbEntry, imgx, imgy, namex, namey, imgscale, menux, menuy);
				}
			}
			else if (pad1.justPressed("B"))
			{
				textstep = (20 - curtextsize) / stepcount;

				stepx = ((320-(dbEntry.shotImg ? (dbEntry.shotImg.getWidth()/2) : 0)) - imgx) / stepcount;
				stepy = ((240-(dbEntry.shotImg ? (dbEntry.shotImg.getHeight()/2) : 0)) - imgy) / stepcount;
				menuxstep = (640 - menux) / stepcount;

				namestepx = (orig_namex - namex) / stepcount;
				namestepy = (orig_namey - namey) / stepcount;

				var goingBack = false;

				for (var k = 0; k < stepcount; ++k)
				{
					pad1.poll();
					if (pad1.justPressed("A"))
					{
						goingBack = true;
						break;
					}
					Video.frameBegin();
					actionMenu.draw(menux,menuy,50,321,436);
					bgimg.draw(0, 0, 1, g_colors["white"], 10);
					if (dbEntry.shotImg)
						dbEntry.shotImg.draw(imgx, imgy, imgscale, g_colors["white"], 100+(imgscale*100));
					main_font.draw(dbEntry.name, namex, namey, 300, -1, curtextsize, g_colors["white"]);
					Video.frameFinish();

					imgx += stepx;
					imgy += stepy;
					namex += namestepx;
					namey += namestepy;
					menux += menuxstep;
					curtextsize += textstep;
					curstep++;
				}
				if (goingBack)
					break;
				return null;
			}

			Video.frameBegin();
			actionMenu.draw(menux,menuy,50,321,436);
			bgimg.draw(0, 0, 1, g_colors["white"], 10);
			if (dbEntry.shotImg)
				dbEntry.shotImg.draw(imgx, imgy, imgscale, g_colors["white"], 100+(imgscale*100));
			main_font.draw(dbEntry.name, namex, namey, 300, -1, curtextsize, g_colors["white"]);
			Video.frameFinish();

			if (curstep < stepcount)
			{
				imgx += stepx;
				imgy += stepy;
				namex += namestepx;
				namey += namestepy;
				menux += menuxstep;
				imgscale += stepscale;
				curtextsize += textstep;
				curstep++;
			}
		}
	}
}

function loadGameScreenNew()
{
	System.garbageCollect();
	
	var loadbg = new Image();
	loadbg.load("/cd/graphics/optionsbg.jpg", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	
	var loadedEntries = new Array(null, null, null, null, null);
	var midIndex = 2;
	
	for (var k = 0; k < loadedEntries.length; ++k)
	{
		var temp = localIndex+(k-midIndex);
		if (temp >= 0 && temp < getDBSize())
			loadedEntries[k] = loadupGameData(getDBRec(temp), g_extraNames[curExtra]);
	}
	
	var tempHalfWidth = 0;
	var animSteps = 15;
	var animCount = animSteps;
	var movingRight = true;
	var midimgx = 0;
	var midnamex = 0;
	var midnamey = 0;
	var midimgscale = 0;
	var speedRight = false;
	var speedLeft = false;
	var speedEnded = false;
	while (true)
	{
		pad1.poll();
		if (pad1.justPressed("A") || pad1.justPressed("START"))
		{
			if (!loadedEntries[midIndex])
				continue;
			var dbEntry = loadedEntries[midIndex];
			var nextFunc = whatToLoadMenu(loadbg, dbEntry, midimgx, 240, 320-loadedEntries[midIndex].halfWidth, 410, midimgscale);
			if (nextFunc)
				return nextFunc;
			animCount = 15;
		}
		else if (pad1.justPressed("B"))
			return mainMenu;
		else if (pad1.justPressed("Y"))
		{
			activateNextList();
			localIndex = 0;
			return loadGameScreenNew;
		}
		else if ((speedLeft || pad1.justPressed("LEFT")) && localIndex > 0)
		{
			var leaving = loadedEntries[loadedEntries.length-1];
			if (leaving && leaving.shotImg)
				leaving.shotImg.unload();
			for (var k = loadedEntries.length-1; k > 0; --k)
				loadedEntries[k] = loadedEntries[k-1];
			localIndex--;
			var temp = localIndex-midIndex;
			if (temp >= 0)
				loadedEntries[0] = loadupGameData(getDBRec(temp), g_extraNames[curExtra], speedLeft);
			else
				loadedEntries[0] = null;
			animCount = 0;
			movingRight = true;
		}
		else if ((speedRight || pad1.justPressed("RIGHT")) && ((localIndex+midIndex)-1) < getDBSize())
		{
			var leaving = loadedEntries[0];
			if (leaving && leaving.shotImg)
				leaving.shotImg.unload();
			for (var k = 1; k < loadedEntries.length; ++k)
				loadedEntries[k-1] = loadedEntries[k];
			localIndex++;
			var temp = localIndex+((loadedEntries.length-1)-midIndex);
			if (temp < getDBSize())
				loadedEntries[loadedEntries.length-1] = loadupGameData(getDBRec(temp), g_extraNames[curExtra], speedRight);
			else
				loadedEntries[loadedEntries.length-1] = null;
			animCount = 0;
			movingRight = false;
		}
		else if (speedEnded || pad1.justPressed("UP"))
		{
			if (!speedEnded)
				curExtra--;
			else
			{
				speedEnded = false;
				animCount = animSteps;
			}
			if (curExtra < 0)
				curExtra = g_extraNames.length-1;
			for (var k = 0; k < loadedEntries.length; ++k)
			{
				if (loadedEntries[k])
				{
					if (loadedEntries[k].shotImg)
						loadedEntries[k].shotImg.unload();
					loadedEntries[k] = loadupGameData(getDBRec(localIndex+(k-midIndex)), g_extraNames[curExtra]);
				}
			}
		}
		else if (pad1.justPressed("DOWN"))
		{
			curExtra++;
			if (curExtra >= g_extraNames.length)
				curExtra = 0;
			for (var k = 0; k < loadedEntries.length; ++k)
			{
				if (loadedEntries[k])
				{
					if (loadedEntries[k].shotImg)
						loadedEntries[k].shotImg.unload();
					loadedEntries[k] = loadupGameData(getDBRec(localIndex+(k-midIndex)), g_extraNames[curExtra]);
				}
			}
		}
		
		if (pad1.RTRIG > 128)
		{
			if (!speedRight)
			{
				for (var k = 0; k < loadedEntries.length; ++k)
				{
					if (loadedEntries[k])
					{
						if (loadedEntries[k].shotImg)
							loadedEntries[k].shotImg.unload();
					}
				}
				speedRight = true;
			}
		}
		else if (speedRight)
		{
			speedRight = false;
			speedEnded = true;
		}
		
		if (pad1.LTRIG > 128)
		{
			if (!speedLeft)
			{
				for (var k = 0; k < loadedEntries.length; ++k)
				{
					if (loadedEntries[k])
					{
						if (loadedEntries[k].shotImg)
							loadedEntries[k].shotImg.unload();
					}
				}
				speedLeft = true;
			}
		}
		else if (speedLeft)
		{
			speedLeft = false;
			speedEnded = true;
		}
		
		Video.frameBegin();
		loadbg.draw(0, 0, 1, g_colors["white"], 10);
		
		for (var k = 0; k < loadedEntries.length; ++k)
		{
			var curEntry = loadedEntries[k];
			if (curEntry && curEntry.shotImg)
			{
				var kanim = 0;
				if (movingRight)
					kanim = (k-1)+(animCount/animSteps);
				else
					kanim = (k+1)-(animCount/animSteps);
				var imgx = Math.floor((452/(loadedEntries.length-1))*kanim)+94;
				var imgscale = 1 - (.5*Math.abs(kanim-midIndex)/midIndex);
				var iColor = Math.floor(255*imgscale);
				curEntry.shotImg.drawRotated(imgx, 240, imgscale, packColor(255, iColor, iColor, iColor), 100+(imgscale*100), 0.5, 0.5, 0);
				if (k == midIndex)
				{
					midimgx = imgx;
					midimgscale = imgscale;
				}
			}
		}
		
		if (speedLeft || speedRight)
			main_font.draw(loadedEntries[midIndex].name.charAt(0), 200, 350, 300, -1, 320, g_colors["white"]);
		
		main_font.draw("View: "+g_extraLongNames[curExtra]+" (Up & Down to change)", 30, 60, 300, -1, testsize, g_colors["white"]);
		main_font.draw(curListName?curListName.substr(5):"All", 500, 60, 300, -1, testsize, g_colors["white"]);
		if (loadedEntries[midIndex])
			main_font.draw(loadedEntries[midIndex].name, 320-loadedEntries[midIndex].halfWidth, 410, 300, -1, testsize, g_colors["white"]);
		Video.frameFinish();
		if (animCount < animSteps)
			animCount++;
	}
}

function loadGameScreenOld()
{
	System.garbageCollect();
	var loadbg = new Image();
	loadbg.load("/cd/graphics/optionsbg.jpg", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	if (!romBrowser)
	{
		romBrowser = new Menu();
		romBrowser.visibleRowCount = 18;
		romBrowser.setColorRule(g_colors["red"], "isDir", true);
		var templist = FileSystem.getFileList(g_settings.normalBrowseDir);
		romBrowser.setItems(templist);
		directoryCache[g_settings.normalBrowseDir] = templist;
	}
	
	while (true)
	{
		pad1.poll();
		romBrowser.processInput();
		if (pad1.justPressed("A"))
		{
			if (romBrowser.curItem)
			{
				if (romBrowser.curItem.isDir)
				{
					g_settings.normalBrowseDir += "/"+romBrowser.curItem.name;
					var templist = null;
					if (directoryCache[g_settings.normalBrowseDir])
						templist = directoryCache[g_settings.normalBrowseDir];
					else
						templist = FileSystem.getFileList(g_settings.normalBrowseDir);
					romBrowser.setItems(templist);
					directoryCache[g_settings.normalBrowseDir] = templist;
				}
				else
				{
					g_romFilename = g_settings.normalBrowseDir+"/"+romBrowser.curItem.name;
					Nester.loadROM(g_romFilename);
					g_loadedGameEntry = null;
					return runGame;
				}
			}
		}
		else if (pad1.justPressed("X"))
		{
			var lastSlashPos = g_settings.normalBrowseDir.lastIndexOf("/");
			if (lastSlashPos > 0 || g_settings.normalBrowseDir != "/")
			{
				g_settings.normalBrowseDir = g_settings.normalBrowseDir.substr(0, lastSlashPos);
				print(g_settings.normalBrowseDir+"\n");
				var templist = null;
				if (directoryCache[g_settings.normalBrowseDir])
					templist = directoryCache[g_settings.normalBrowseDir];
				else
					templist = FileSystem.getFileList(g_settings.normalBrowseDir);
				romBrowser.setItems(templist);
				directoryCache[g_settings.normalBrowseDir] = templist;
			}
		}
		else if (pad1.justPressed("Y"))
		{
			saveSettings();
			return loadGameScreenNew;
		}
		else if (pad1.justPressed("B"))
			return mainMenu;
		Video.frameBegin();
		loadbg.draw(0, 0, 1, g_colors["white"], 10);
		romBrowser.draw(16,16,50,321,436);
		Video.frameFinish();
	}
}

function runGame()
{
	System.garbageCollect();
	Nester.autoStart(g_settings.nesModeNTSC?60:50);
	while (true)
	{
		var autoret = Nester.autoLoop();
		if (autoret == 1)
			return inGameMenu;
		else if (autoret == 2)
			continue;
		Video.frameBegin();
		if (g_settings.renderTrue)
			Nester.drawTrue();
		else
			Nester.draw();
		if (g_settings.showFPS)
		{
			main_font.draw(Nester.fps+"/"+dcfps, 50, 50, 100, -1, 30, g_colors["white"]);
			Video.drawRect(46, 23, 137, 55, 80, g_colors["black"]);
		}
		Video.frameFinish();
	}
	Nester.autoEnd();
}

function inGameMenu()
{
	System.garbageCollect();
	var inMenu = new Menu();
	inMenu.fontSize = 30;
	inMenu.visibleRowCount = 12;
	inMenu.items = new Array();
	inMenu.items[inMenu.items.length] = {name:"Return To Game", func:runGame};
	inMenu.items[inMenu.items.length] = {name:"Reset", isReset:true};
	if (Nester.hasSRAM)
		inMenu.items[inMenu.items.length] = {name:"Save SRAM", isSaveSRAM:true};
	inMenu.items[inMenu.items.length] = {name:"State Slot: "+Nester.saveSlot, isSaveSlot:true};
	inMenu.items[inMenu.items.length] = {name:"Save State", isSaveState:true};
	inMenu.items[inMenu.items.length] = {name:"Load State", isLoadState:true};
	inMenu.items[inMenu.items.length] = {name:"Game Genie", isGameGenie:true};
	inMenu.items[inMenu.items.length] = {name:"Edit Notes", isEditNotes:true};
	if (!g_settings.renderTrue)
		inMenu.items[inMenu.items.length] = {name:"Adjust Screen", isAdjustScreen:true};
	inMenu.items[inMenu.items.length] = {name:"Main Menu", func:mainMenu};
	var iColor = 255;
	var iColor2 = 255;
	while (true)
	{
		pad1.poll();
		inMenu.processInput();
		if (pad1.justPressed("A"))
		{
			if (inMenu.curItem.func)
				return inMenu.curItem.func;
			if (inMenu.curItem.isSaveState)
			{
				Nester.saveState();
				break;
			}
			else if (inMenu.curItem.isLoadState)
			{
				Nester.loadState();
				break;
			}
			else if (inMenu.curItem.isSaveSRAM)
			{
				Nester.saveSRAM();
				break;
			}
			else if (inMenu.curItem.isAdjustScreen)
				adjustScreen(null);
			else if (inMenu.curItem.isGameGenie)
			{
				gameGenieScreen();
				System.garbageCollect();
				continue;
			}
			else if (inMenu.curItem.isEditNotes)
			{
				var vmuPath = findFirstVMU();
				if (vmuPath)
				{
					var notePath = vmuPath+"/"+Nester.getROMCRCstr()+".txt";
					var str = "";
					var f = new VMUFile();
					if (f.load(notePath))
						str = f.data;
					str = viewText(str, null, 255, true);
					if (str == "")
						FileSystem.remove(notePath);
					else
					{
						if (g_loadedGameEntry)
							f.longDesc = g_loadedGameEntry.name;
						else
							f.longDesc = "Some Game";
						f.shortDesc = "Notes";
						f.appID = "NesterDC SE";
						f.data = str;
						f.save(notePath);
					}
					System.garbageCollect();
					continue;
				}
			}
			else if (inMenu.curItem.isReset)
			{
				Nester.softReset();
				break;
			}
		}
		if (pad1.justPressed("RIGHT"))
		{
			if (inMenu.curItem.isSaveSlot)
			{
				Nester.saveSlot++;
				if (Nester.saveSlot > 9)
					Nester.saveSlot = 0;
				inMenu.curItem.name = "Save Slot: "+Nester.saveSlot;
			}
		}
		if (pad1.justPressed("LEFT"))
		{
			if (inMenu.curItem.isSaveSlot)
			{
				Nester.saveSlot--;
				if (Nester.saveSlot < 0)
					Nester.saveSlot = 9;
				inMenu.curItem.name = "Save Slot: "+Nester.saveSlot;
			}
		}
		
		if (pad1.justPressed("B") || pad1.justPressed("Y"))
			break;
		Video.frameBegin();
		if (g_settings.renderTrue)
			Nester.drawTrue(packColor(255, iColor, iColor, iColor));
		else
			Nester.draw(packColor(255, iColor, iColor, iColor));
		inMenu.draw(16,16,50,622,461);
		Video.frameFinish();
		if (iColor > 64)
		{
			iColor -= 3;
			iColor2--;
		}
	}
	return runGame;
}

function mainMenu()
{
	System.garbageCollect();
	
	var mainbg = new Image();
	var redCircle = new Image();
	var iconOptions = new Image();
	var iconLoadGame = new Image();
	
	mainbg.load("/cd/graphics/mainbg.jpg", IMG_OPAQUE, IMG_FILTER_NONE, IMG_ALPHA_NONE);
	redCircle.load("/cd/graphics/red_circle.png", IMG_PUNCH_THRU, IMG_FILTER_NONE, IMG_ALPHA_KEYED);
	iconOptions.load("/cd/graphics/controller_sp.png", IMG_PUNCH_THRU, IMG_FILTER_NONE, IMG_ALPHA_KEYED);
	iconLoadGame.load("/cd/graphics/cart_sp.png", IMG_PUNCH_THRU, IMG_FILTER_NONE, IMG_ALPHA_KEYED);
	
	var iconArray = new Array(iconLoadGame, iconOptions);
	var captionArray = new Array("Browse Library", "Options And More");
	
	var frameCount = 0;
	var gigglex = 0;
	var giggley = 0;
	
	while (true)
	{
		pad1.poll();
		
		var oldSelected = selectedIcon;
		if (pad1.justPressed("A"))
		{
			switch (selectedIcon)
			{
				case 0:
				{
					if (g_settings.romBrowseMode == 'SE')
						return loadGameScreenNew;
					else
						return loadGameScreenOld;
				}
				case 1: return optionsScreen;
			};
		}
		if (pad1.justPressed("B") && g_romFilename)
			return inGameMenu;
		if (pad1.justPressed("RIGHT"))
		{
			if (selectedIcon == 0)
				selectedIcon++;
		}
		if (pad1.justPressed("LEFT"))
		{
			if (selectedIcon == 1)
				selectedIcon--;
		}
		if (selectedIcon != oldSelected)
			frameCount = 0;
		
		Video.frameBegin();
		
		var tempStep = (frameCount < 30) ? frameCount : 30;
		var tempFontSize = tempStep;
		var tempHalfWidth = main_font.getWidth(captionArray[selectedIcon], tempFontSize) / 2;
		var textCurX = 0;
		var textCurY = 0;
		switch (selectedIcon)
		{
			case 0:
				textCurX = 142;
				textCurY = 314;
				break;
			case 1:
				textCurX = 489;
				textCurY = 314;
				break;
		}
		textCurX = (textCurX + (((320 - textCurX) / 30) * tempStep))-tempHalfWidth;
		textCurY = (textCurY + (((200 - textCurY) / 30) * tempStep));
		if (tempStep < 30)
			textCurY += Math.sin((tempStep / 30) * Math.PI) * 100;
		main_font.draw(captionArray[selectedIcon], textCurX, textCurY, ((tempStep == 30) ? 100 : 15), -1, tempFontSize, g_colors["white"]);
		
		mainbg.drawWH(0, 0, 640, 480, g_colors["white"], 10);
		
		if ((frameCount%8) == 0)
		{
			gigglex = Math.floor((Math.random()*8)-4);
			giggley = Math.floor((Math.random()*8)-4);
		}
		
		for (var k = 0; k < iconArray.length; ++k)
		{
			var x = 75;
			var y = 210;
			var icon = iconArray[k];
			if ((k%2) == 1)
				x += 285;
			
			redCircle.draw(x, y, 1, g_colors["white"], 17);
			if (k == selectedIcon)
			{
				x += gigglex;
				y += giggley;
			}
			icon.draw(x, y, 1, g_colors["white"], 20);
		}
		
		Video.frameFinish();
		
		frameCount++;
	}
}

function main()
{
	println("NesterDC SE");
	
	loadSettings();
	
	//var newstr = viewText("test\nstuff", null, 255, true);
	//System.garbageCollect();
	
	//g_romFilename = "/cd/Snake Rattle'n Roll (E).nes";
	//Nester.loadROM(g_romFilename);
	//var actionFunc = runGame;
	
	titleScreen();
	delete titleScreen;
	
	var actionFunc = mainMenu;
	while (true)
		actionFunc = actionFunc();
	print("DONE\n");
}
