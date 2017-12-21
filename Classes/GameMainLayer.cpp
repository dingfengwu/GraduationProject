#include "GameMainLayer.h"

GameMainLayer::GameMainLayer()
{

}

GameMainLayer::~GameMainLayer()
{

}

// set 痕方鹿

void GameMainLayer::setPlayersNumber(int pn)
{
	this->playersNumber = pn;
}

void GameMainLayer::setStartMoney(int sm)
{
	this->startMoney = sm;
}

void GameMainLayer::setPlayersRoles(vector<int> pr)
{
	for (int i : pr)
	{
		this->roles.push_back(i);
	}
}

void GameMainLayer::setMapNumber(int mn)
{
	this->mapNumber = mn;
}

// schedule �犢愃�方

void GameMainLayer::removeParticle(float dt)
{
	auto p = this->getChildByName("particle");
	p->removeFromParentAndCleanup(true);
}

void GameMainLayer::continueLoading(float dt)
{
	auto l = (Label*)(this->getChildByName("loading"));
	l->setString(l->getString() + " .");
}

void GameMainLayer::removeLoading(float dt)
{
	auto a = this->getChildByName("loading");
	a->removeFromParentAndCleanup(true);
}

// init
bool GameMainLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	// 兜兵晒欺及匯了螺社
	nowPlayerNumber = 1;

	// 兜兵晒融窟並周方楚
	emEventNumber = 5;

	// 圓譜崔蝕兵議輿仇吉雫方
	landLevelNumber = 8;
	
	// 指栽方
	rounds = 1;
	
	// Nowloading
	Label* loading = Label::create("Now Loading", "arial", 30);
	loading->enableBold();
	loading->setPosition(visibleSize / 2);
	this->addChild(loading, 5, "loading");

	this->schedule(schedule_selector(GameMainLayer::continueLoading), 0.5, 3, 0.5);

	this->scheduleOnce(schedule_selector(GameMainLayer::removeLoading), 2);
	// ここまでだ

	// 蝕兵麼痕方
	this->scheduleOnce(schedule_selector(GameMainLayer::setParameter), 2);

	return true;
}

void GameMainLayer::setParameter(float dt)
{
	this->createMap();
	this->createPlayer();
	this->createPlayerPro();
}

// 兜兵晒�犢愃�方

void GameMainLayer::createMap()
{
	string str = "tmx/map" + to_string(mapNumber) + ".tmx";
	tileMap = TMXTiledMap::create(str);

	this->addChild(tileMap);

	road = tileMap->getLayer("Road");

	for (int i = 1; i <= landLevelNumber; i++)
	{
		string s = "Land" + to_string(i);
		TMXLayer* tl = tileMap->getLayer(s);
		lands.push_back(tl);
	}

	// 悳指栽方
	auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");
	const char* roundsC = ((String*)ngContent->objectForKey("rounds"))->getCString();
	Label* roundsL1 = Label::createWithSystemFont(roundsC, "arial", 24);
	roundsL1->enableBold();
	roundsL1->setPosition(visibleSize.width * 3 / 8, visibleSize.height - roundsL1->getContentSize().height);
	this->addChild(roundsL1, 10);

	Label* roundsL2 = Label::createWithSystemFont(to_string(rounds), "arial", 18);
	roundsL2->enableBold();
	roundsL2->setPosition(visibleSize.width * 3 / 8, roundsL1->getPosition().y - roundsL2->getContentSize().height);
	this->addChild(roundsL2, 10, "roundsL");;
}

void GameMainLayer::createPlayer()
{
	objectGroup = tileMap->getObjectGroup("Object");
	auto player = objectGroup->getObject("player");
	float px = player["x"].asFloat();
	float py = player["y"].asFloat();

	for (int i = 1; i <= playersNumber; i++)
	{
		Player p;
		p.name = "player" + to_string(i);
		p.isGoing = false;
		p.faceTo = faceForward::right;
		p.rolePosition = Vec2(4, 4);
		p.roleSprite = Sprite::create("image/role" + to_string(roles[i - 1]) + "_right.png");
		p.money = startMoney;
		p.state = stateType::normal;
		p.spritePosition = Vec2(px + i * 3, py);
		p.roleSprite->setPosition(p.spritePosition);

		if (i == 1)
		{
			p.color = Color3B::RED;
		}
		else if (i == 2)
		{
			p.color = Color3B::BLUE;
		}
		else if (i == 3)
		{
			p.color = Color3B::BLACK;
		}
		else if (i == 4)
		{
			p.color = Color3B::MAGENTA;
		}

		tileMap->addChild(p.roleSprite, tileMap->getChildrenCount(), "player" + to_string(i));

		players.push_back(p);
	}
}

void GameMainLayer::createPlayerPro()
{
	auto ng = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

	int times = 0;

	for (auto p : players)
	{
		// 螺社侖兆
		const char* pc = ((String*)ng->objectForKey(p.name))->getCString();
		Label* l1 = Label::createWithSystemFont(pc, "arial", 20);
		l1->setColor(p.color);
		l1->enableBold();
		l1->setPosition(visibleSize.width * 6 / 7,
			visibleSize.height - visibleSize.height * 3 * times / 16 - l1->getContentSize().height / 2);
		this->addChild(l1, 2, pc);

		// 螺社署熱
		const char* rmb = ((String*)ng->objectForKey("rmb"))->getCString();
		string m = rmb + to_string(p.money);
		string blank = " ";
		Label* l2 = Label::createWithSystemFont(m, "arial", 18);
		l2->enableBold();
		l2->setPosition(visibleSize.width * 6 / 7, l1->getPosition().y - 2 * l1->getContentSize().height);
		this->addChild(l2, 2, p.name + blank + "money");

		times++;
	}

	// �思唹甘�
	Button* diceButton = Button::create("image/diceButtonNormal.png", "image/diceButtonPressed.png");
	diceButton->setPosition(Vec2(visibleSize.width - diceButton->getContentSize().width / 2,
		diceButton->getContentSize().height / 2));
	diceButton->addTouchEventListener(CC_CALLBACK_2(GameMainLayer::diceEvent, this));
	diceButton->setPressedActionEnabled(true);
	this->addChild(diceButton, 2);

	// 譜崔梓泥
	Button* setButton = Button::create("image/settingButtonNormal.png", "image/settingButtonPressed.png");
	setButton->setPosition(Vec2(setButton->getContentSize().width,
		visibleSize.height - setButton->getContentSize().height));
	setButton->addTouchEventListener(CC_CALLBACK_2(GameMainLayer::setEvent, this));
	setButton->setPressedActionEnabled(true);
	this->addChild(setButton, 2);

	// 購液梓泥
	Button* exitButton = Button::create("image/exitButtonNormal.png", "image/exitButtonPressed.png");
	exitButton->setPosition(Vec2(exitButton->getContentSize().width,
		visibleSize.height - exitButton->getContentSize().height * 2.5));
	exitButton->addTouchEventListener(CC_CALLBACK_2(GameMainLayer::exitEvent, this));
	exitButton->setPressedActionEnabled(true);
	this->addChild(exitButton, 2);

	/*
	// 諒籾梓泥
	Button* questionButton = Button::create("image/settingButtonNormal.png", "image/settingButtonPressed.png");
	questionButton->setPosition(Vec2(questionButton->getContentSize().width,
		visibleSize.height - questionButton->getContentSize().height * 3));
	questionButton->addTouchEventListener(CC_CALLBACK_2(GameMainLayer::setEvent, this));
	questionButton->setPressedActionEnabled(true);
	this->addChild(questionButton, 2);*/
}

// 強恬�犢愃�方
void GameMainLayer::diceEvent(Ref* pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		break;
	case Widget::TouchEventType::MOVED:
		break;
	case Widget::TouchEventType::ENDED:
	{
		int n = 1;

		for (auto& p : players)
		{
			if (n == nowPlayerNumber)
			{
				if (p.isGoing == false)
				{
					if (this->checkState())
					{
						int i = random(1, 6);

						switch (i)
						{
						case 1:
							dicePointS = Sprite::create("image/point1.png");
							break;
						case 2:
							dicePointS = Sprite::create("image/point2.png");
							break;
						case 3:
							dicePointS = Sprite::create("image/point3.png");
							break;
						case 4:
							dicePointS = Sprite::create("image/point4.png");
							break;
						case 5:
							dicePointS = Sprite::create("image/point5.png");
							break;
						case 6:
							dicePointS = Sprite::create("image/point6.png");
							break;
						}
						dicePointS->setPosition(visibleSize.width / 2, visibleSize.height / 2);
						this->addChild(dicePointS, 5, "dicePoint");

						p.isGoing = true;

						this->schedule(schedule_selector(GameMainLayer::playerGo), 0.5, i - 1, 0);
						this->scheduleOnce(schedule_selector(GameMainLayer::checkRoad), i*0.5);
					}
					else
					{
						// (℃-ｖ-)
					}
				}

				break;
			}

			n++;
		}

		break;
	}
	case Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void GameMainLayer::setEvent(Ref* pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		break;
	case Widget::TouchEventType::MOVED:
		break;
	case Widget::TouchEventType::ENDED:
	{
		auto gsl = GameSetLayer::create();
		this->addChild(gsl, 100);
	}
	case Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void GameMainLayer::exitEvent(Ref* pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		break;
	case Widget::TouchEventType::MOVED:
		break;
	case Widget::TouchEventType::ENDED:
	{
		auto btt = BackToTitleLayer::create();
		this->addChild(btt, 100);
	}
	case Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void GameMainLayer::playerGo(float dt)
{
	int n = 1;

	for (auto& nowPlayer : players)
	{
		if (n == nowPlayerNumber)
		{
			if (road->getTileAt(nowPlayer.rolePosition))
			{
				if (nowPlayer.faceTo == faceForward::right)
				{
					if (road->getTileAt(Vec2(nowPlayer.rolePosition.x + 1, nowPlayer.rolePosition.y)))
					{
						nowPlayer.rolePosition.x++;
						nowPlayer.spritePosition.x += 30;
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x, nowPlayer.rolePosition.y + 1)))
					{
						nowPlayer.rolePosition.y++;
						nowPlayer.spritePosition.y -= 30;
						nowPlayer.faceTo = faceForward::down;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_down.png");
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x, nowPlayer.rolePosition.y - 1)))
					{
						nowPlayer.rolePosition.y--;
						nowPlayer.spritePosition.y += 30;
						nowPlayer.faceTo = faceForward::up;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_up.png");
					}
				}
				else if (nowPlayer.faceTo == faceForward::down)
				{
					if (road->getTileAt(Vec2(nowPlayer.rolePosition.x, nowPlayer.rolePosition.y + 1)))
					{
						nowPlayer.rolePosition.y++;
						nowPlayer.spritePosition.y -= 30;
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x + 1, nowPlayer.rolePosition.y)))
					{
						nowPlayer.rolePosition.x++;
						nowPlayer.spritePosition.x += 30;
						nowPlayer.faceTo = faceForward::right;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_right.png");
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x - 1, nowPlayer.rolePosition.y)))
					{
						nowPlayer.rolePosition.x--;
						nowPlayer.spritePosition.x -= 30;
						nowPlayer.faceTo = faceForward::left;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_left.png");
					}
				}
				else if (nowPlayer.faceTo == faceForward::left)
				{
					if (road->getTileAt(Vec2(nowPlayer.rolePosition.x - 1, nowPlayer.rolePosition.y)))
					{
						nowPlayer.rolePosition.x--;
						nowPlayer.spritePosition.x -= 30;
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x, nowPlayer.rolePosition.y + 1)))
					{
						nowPlayer.rolePosition.y++;
						nowPlayer.spritePosition.y -= 30;
						nowPlayer.faceTo = faceForward::down;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_down.png");
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x, nowPlayer.rolePosition.y - 1)))
					{
						nowPlayer.rolePosition.y--;
						nowPlayer.spritePosition.y += 30;
						nowPlayer.faceTo = faceForward::up;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_up.png");
					}
				}
				else if (nowPlayer.faceTo == faceForward::up)
				{
					if (road->getTileAt(Vec2(nowPlayer.rolePosition.x, nowPlayer.rolePosition.y - 1)))
					{
						nowPlayer.rolePosition.y--;
						nowPlayer.spritePosition.y += 30;
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x + 1, nowPlayer.rolePosition.y)))
					{
						nowPlayer.rolePosition.x++;
						nowPlayer.spritePosition.x += 30;
						nowPlayer.faceTo = faceForward::right;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_right.png");
					}
					else if (road->getTileAt(Vec2(nowPlayer.rolePosition.x - 1, nowPlayer.rolePosition.y)))
					{
						nowPlayer.rolePosition.x--;
						nowPlayer.spritePosition.x -= 30;
						nowPlayer.faceTo = faceForward::left;
						nowPlayer.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_left.png");
					}
				}
			}

			nowPlayer.roleSprite->setPosition(nowPlayer.spritePosition);

			break;
		}
		n++;
	}
}

bool GameMainLayer::checkState()
{
	// 忖隅断
	auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

	int n = 1;

	for (auto& p : players)
	{
		if (n == nowPlayerNumber)
		{
			if (p.state == stateType::normal)
			{
				// 屎械佩恠
				return true;
			}
			else if (p.state == stateType::parking && p.stayRound > 0)
			{
				// 写偬棋壓唯概魁
				p.isGoing = true;

				p.stayRound--;

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanAndChange, this));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* roundsToStayInParkinglot1 = ((String*)ngContent->objectForKey("roundsToStayInParkinglot1"))->getCString();
				const char* roundsToStayInParkinglot2 = ((String*)ngContent->objectForKey("roundsToStayInParkinglot2"))->getCString();
				string blank = " ";
				string s = roundsToStayInParkinglot1 + blank + "\n" + roundsToStayInParkinglot2 + to_string(p.stayRound);
				Label* noticeL = Label::createWithSystemFont(s, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);
				noticeL->setMaxLineWidth(520);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);

				return false;
			}
			else if (p.state == stateType::prison && p.stayRound > 0)
			{
				// 写偬棋壓酌囿
				p.isGoing = true;

				p.stayRound--;

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanAndChange, this));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* roundsToStayInPrison1 = ((String*)ngContent->objectForKey("roundsToStayInPrison1"))->getCString();
				const char* roundsToStayInPrison2 = ((String*)ngContent->objectForKey("roundsToStayInPrison2"))->getCString();
				string blank = " ";
				string s = roundsToStayInPrison1 + blank + "\n" + roundsToStayInPrison2 + to_string(p.stayRound);
				Label* noticeL = Label::createWithSystemFont(s, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);
				noticeL->setMaxLineWidth(520);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);

				return false;
			}
			else if (p.state == stateType::parking && p.stayRound == 0)
			{
				// 唯概魁扮寂欺��竃窟

				p.state = stateType::normal;

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanMenu, this));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* outOfParkinglot = ((String*)ngContent->objectForKey("outOfParkinglot"))->getCString();
				Label* noticeL = Label::createWithSystemFont(outOfParkinglot, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);

				return true;
			}
			else if (p.state == stateType::prison && p.stayRound == 0)
			{
				// 瑩瑣扮寂欺��竃囿

				p.state = stateType::normal;

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanMenu, this));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* outOfPrison = ((String*)ngContent->objectForKey("outOfPrison"))->getCString();
				Label* noticeL = Label::createWithSystemFont(outOfPrison, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);

				return true;
			}
		}

		n++;
	}
}

void GameMainLayer::checkRoad(float dt)
{
	// 賠茅�思嗟柴�
	this->removeChildByName("dicePoint");

	auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

	int n = 1;

	for (auto& p : players)
	{
		if (n == nowPlayerNumber)
		{
			if (road->getTileGIDAt(p.rolePosition) == normal_road_GID)
			{
				// 屎械宥狛
				this->scheduleOnce(schedule_selector(GameMainLayer::checkLand), 0.1);
			}
			else if (road->getTileGIDAt(p.rolePosition) == prisonEntrance_road_GID)
			{
				// 琴酌囿

				// 俐個輝念螺社議彜蓑、了崔才夕頭了崔
				p.state = stateType::prison;
				p.rolePosition = Vec2(4, 17);
				p.spritePosition.x -= 17 * 30;
				p.spritePosition.y -= 14 * 30;
				p.roleSprite->setPosition(p.spritePosition);
				p.roleSprite->setTexture("image/role" + to_string(roles[n - 1]) + "_up.png");
				p.faceTo = faceForward::up;
				p.stayRound = 3;

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanAndChange, this));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* prisonEnterance1 = ((String*)ngContent->objectForKey("prisonEnterance1"))->getCString();
				const char* prisonEnterance2 = ((String*)ngContent->objectForKey("prisonEnterance2"))->getCString();
				string blank = " ";
				string s = prisonEnterance1 + blank + "\n" + prisonEnterance2;
				Label* noticeL = Label::createWithSystemFont(s, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);
				noticeL->setMaxLineWidth(520);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);
			}
			else if (road->getTileGIDAt(p.rolePosition) == prison_road_GID)
			{
				// 匯違宥狛酌囿
				this->changePlayer();
			}
			else if (road->getTileGIDAt(p.rolePosition) == parkinglot_road_GID)
			{
				// 是壓唯概魁
				p.state = stateType::parking;
				p.stayRound = 2;

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanAndChange, this));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* parkinglot = ((String*)ngContent->objectForKey("parkinglot"))->getCString();
				Label* noticeL = Label::createWithSystemFont(parkinglot, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);
				noticeL->setMaxLineWidth(520);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);
			}
			else if (road->getTileGIDAt(p.rolePosition) == emergency_road_GID)
			{
				// 融窟並周

				// 並周窃侏
				int i = random(1, emEventNumber);
				int loss;

				switch (i)
				{
				case 1:
					loss = 100;
					break;
				case 2:
					loss = 300;
					break;
				case 3:
					loss = 500;
					break;
				case 4:
					loss = 1000;
					break;
				case 5:
					loss = 2000;
					break;
				}

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::checkPayPlayerMoneyLo, this, p.name, loss));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* emergencyEvent = ((String*)ngContent->objectForKey("emergencyEvent" + to_string(i)))->getCString();
				Label* noticeL = Label::createWithSystemFont(emergencyEvent, "arial", 25);

				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);
			}
			else if (road->getTileGIDAt(p.rolePosition) == tax_road_GID)
			{
				// 住弌継

				// 暇汽中医夕頭
				menuBoard = Sprite::create("image/Popup.png");
				menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
				this->addChild(menuBoard);

				// 暇汽�哉袈�
				MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
					"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::checkPayPlayerMoneyLo, this, p.name, 700));
				okM->setPosition(0, -visibleSize.height / 5);

				const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
				Label* okL = Label::createWithSystemFont(okC, "arial", 20);
				okM->addChild(okL);
				okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
				okL->setTextColor(Color4B::BLACK);

				// 暇汽麼勣坪否
				const char* tax1 = ((String*)ngContent->objectForKey("tax1"))->getCString();
				const char* tax2 = ((String*)ngContent->objectForKey("tax2"))->getCString();
				string blank = " ";
				string s = tax1 + blank + "\n" + tax2;
				Label* noticeL = Label::createWithSystemFont(s, "arial", 25);


				menuBoard->addChild(noticeL);
				noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
				noticeL->setTextColor(Color4B::BLACK);

				noticeMenu = Menu::create(okM, NULL);
				this->addChild(noticeMenu);
			}
			else if (road->getTileGIDAt(p.rolePosition) == entrance_road_GID)
			{
				// 匯違宥狛軟兵了崔
				this->changePlayer();
			}

			break;
		}

		n++;
	}
}

void GameMainLayer::checkLand(float dt)
{
	int n = 1;

	for (auto& p : players)
	{
		if (n == nowPlayerNumber)
		{
			// 登僅繁麗劾�鬘�鳩協輿仇了崔
			switch (p.faceTo)
			{
			case faceForward::right:
				nowLand = Vec2(p.rolePosition.x, p.rolePosition.y + 1);
				break;
			case faceForward::down:
				nowLand = Vec2(p.rolePosition.x - 1, p.rolePosition.y);
				break;
			case faceForward::left:
				nowLand = Vec2(p.rolePosition.x, p.rolePosition.y - 1);
				break;
			case faceForward::up:
				nowLand = Vec2(p.rolePosition.x + 1, p.rolePosition.y);
				break;
			}

			// 貫侭嗤輿仇蚊登僅頁倦壓斤哘恫炎贋壓輿仇
			for (auto l : lands)
			{
				sLand = l->getTileAt(nowLand);

				if (sLand)
				{
					gLand = l->getTileGIDAt(nowLand);

					break;
				}
			}

			if (sLand)
			{
				if (gLand == empty_land_GID && sLand->getColor() == Color3B::WHITE)
				{
					this->emptyLand();
				}
				else if (sLand->getColor() == p.color)
				{
					this->myLand();
				}
				else
				{
					this->otherLand();
				}
			}
			else
			{
				this->changePlayer();
			}

			break;
		}

		n++;
	}
}

// Land�犢愃�方

// 腎仇
void GameMainLayer::emptyLand()
{
	string name;
	int n = 1;

	for (auto p : players)
	{
		if (n == nowPlayerNumber)
		{
			name = p.name;
			break;
		}

		n++;
	}

	// 暇汽中医夕頭
	menuBoard = Sprite::create("image/Popup.png");
	menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(menuBoard);

	// 忖隅断
	auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

	// 暇汽 �妻�
	MenuItem* yesM = MenuItemImage::create("image/GreenNormal.png",
		"image/GreenPressed.png", CC_CALLBACK_0(GameMainLayer::emptyMenuYes, this));
	yesM->setPosition(-visibleSize.width / 8, -visibleSize.height / 5);

	const char* yesC = ((String*)ngContent->objectForKey("yes"))->getCString();
	Label* yesL = Label::createWithSystemFont(yesC, "arial", 20);
	yesM->addChild(yesL);
	yesL->setPosition(yesM->getContentSize().width / 2, yesM->getContentSize().height / 2);
	yesL->setTextColor(Color4B::BLACK);

	// 暇汽 �嵯�
	MenuItem* noM = MenuItemImage::create("image/RedNormal.png",
		"image/RedPressed.png", CC_CALLBACK_0(GameMainLayer::emptyMenuNo, this));
	noM->setPosition(visibleSize.width / 8, -visibleSize.height / 5);

	const char* noC = ((String*)ngContent->objectForKey("no"))->getCString();
	Label* noL = Label::createWithSystemFont(noC, "arial", 20);
	noM->addChild(noL);
	noL->setPosition(noM->getContentSize().width / 2, noM->getContentSize().height / 2);
	noL->setTextColor(Color4B::BLACK);


	// 暇汽麼勣坪否
	Label* noticeL;

	const char* nc = ((String*)ngContent->objectForKey(name))->getCString();
	string blank = " ";
	const char* comma = ((String*)ngContent->objectForKey("comma"))->getCString();
	const char* upgradeLand0 = ((String*)ngContent->objectForKey("upgradeLand0"))->getCString();
	const char* payToUpgrade = ((String*)ngContent->objectForKey("payToUpgrade"))->getCString();

	// 侭壓腎易仇翠議勺鯉
	int emptyBuildCost;

	for (auto l : lands)
	{
		if (l->getTileAt(nowLand))
		{
			emptyBuildCost = l->getProperty("emptyBuildCost").asInt();
		}
	}

	const char* yuan = ((String*)ngContent->objectForKey("yuan"))->getCString();
	string s = nc + blank + comma + upgradeLand0 + "\n" +
		payToUpgrade + blank + to_string(emptyBuildCost) + yuan;
	noticeL = Label::createWithSystemFont(s, "arial", 25);

	menuBoard->addChild(noticeL);
	noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
	noticeL->setTextColor(Color4B::BLACK);

	noticeMenu = Menu::create(yesM, noM, NULL);
	this->addChild(noticeMenu);
}

void GameMainLayer::emptyMenuYes()
{
	int n = 1;

	for (auto& p : players)
	{
		if (n == nowPlayerNumber)
		{
			int emptyBuildCost;

			for (auto l : lands)
			{
				if (l->getTileAt(nowLand))
				{
					emptyBuildCost = l->getProperty("emptyBuildCost").asInt();
					auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

					if (p.money >= emptyBuildCost)
					{
						l->getTileAt(nowLand)->setColor(p.color);

						// 腺徨�塋�
						ParticleGalaxy* pg = ParticleGalaxy::create();

						switch (p.faceTo)
						{
						case faceForward::left:
							pg->setPosition(p.spritePosition.x, p.spritePosition.y + 30);
							break;
						case faceForward::down:
							pg->setPosition(p.spritePosition.x - 30, p.spritePosition.y);
							break;
						case faceForward::right:
							pg->setPosition(p.spritePosition.x, p.spritePosition.y - 30);
							break;
						case faceForward::up:
							pg->setPosition(p.spritePosition.x + 30, p.spritePosition.y);
							break;
						}
						pg->setStartColor(Color4F::Color4F(p.color.r / 255, p.color.g / 255, p.color.b / 255, 255));
						pg->setEndColor(Color4F::Color4F(0, 0, 0, 0));
						this->addChild(pg, 0, "particle");

						this->scheduleOnce(schedule_selector(GameMainLayer::removeParticle), 1.0f);

						// ここまでだ

						p.money -= emptyBuildCost;

						const char* rmb = ((String*)ngContent->objectForKey("rmb"))->getCString();
						string blank = " ";
						string s = rmb + blank + to_string(p.money);

						((Label*)this->getChildByName(p.name + blank + "money"))->setString(s);

						this->cleanAndChange();
					}
					else
					{
						this->cleanMenu();

						// 暇汽中医夕頭
						menuBoard = Sprite::create("image/Popup.png");
						menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
						this->addChild(menuBoard);

						// 暇汽�哉袈�
						MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
							"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanAndChange, this));
						okM->setPosition(0, -visibleSize.height / 5);

						const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
						Label* okL = Label::createWithSystemFont(okC, "arial", 20);
						okM->addChild(okL);
						okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
						okL->setTextColor(Color4B::BLACK);

						// 暇汽麼勣坪否
						const char* lackOfMoney = ((String*)ngContent->objectForKey("lackOfMoney"))->getCString();
						Label* noticeL = Label::createWithSystemFont(lackOfMoney, "arial", 25);

						menuBoard->addChild(noticeL);
						noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
						noticeL->setTextColor(Color4B::BLACK);

						noticeMenu = Menu::create(okM, NULL);
						this->addChild(noticeMenu);
					}

					break;
				}
			}

			break;
		}

		n++;
	}
}

void GameMainLayer::emptyMenuNo()
{
	this->cleanAndChange();
}

// 厘議
void GameMainLayer::myLand()
{
	string name;
	int n = 1;

	for (auto p : players)
	{
		if (n == nowPlayerNumber)
		{
			name = p.name;
			break;
		}

		n++;
	}

	// 暇汽中医夕頭
	menuBoard = Sprite::create("image/Popup.png");
	menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(menuBoard);

	// 忖隅断
	auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

	// 暇汽 �妻�
	MenuItem* yesM = MenuItemImage::create("image/GreenNormal.png",
		"image/GreenPressed.png", CC_CALLBACK_0(GameMainLayer::myMenuYes, this));
	yesM->setPosition(-visibleSize.width / 8, -visibleSize.height / 5);

	const char* yesC = ((String*)ngContent->objectForKey("yes"))->getCString();
	Label* yesL = Label::createWithSystemFont(yesC, "arial", 20);
	yesM->addChild(yesL);
	yesL->setPosition(yesM->getContentSize().width / 2, yesM->getContentSize().height / 2);
	yesL->setTextColor(Color4B::BLACK);

	// 暇汽 �嵯�
	MenuItem* noM = MenuItemImage::create("image/RedNormal.png",
		"image/RedPressed.png", CC_CALLBACK_0(GameMainLayer::myMenuNo, this));
	noM->setPosition(visibleSize.width / 8, -visibleSize.height / 5);

	const char* noC = ((String*)ngContent->objectForKey("no"))->getCString();
	Label* noL = Label::createWithSystemFont(noC, "arial", 20);
	noM->addChild(noL);
	noL->setPosition(noM->getContentSize().width / 2, noM->getContentSize().height / 2);
	noL->setTextColor(Color4B::BLACK);

	// 戻幣生坪否
	Label* noticeL;
	const char* nc = ((String*)ngContent->objectForKey(name))->getCString();
	string blank = " ";
	const char* comma = ((String*)ngContent->objectForKey("comma"))->getCString();
	const char* upgradeLand;
	int price;

	if (gLand == empty_land_GID)
	{
		upgradeLand = ((String*)ngContent->objectForKey("upgradeLand1"))->getCString();

		for (auto l : lands)
		{
			if (l->getTileAt(nowLand))
			{
				price = l->getProperty("level1BuildCost").asInt();
			}
		}
	}
	else if (gLand == level1_land_GID)
	{
		upgradeLand = ((String*)ngContent->objectForKey("upgradeLand2"))->getCString();

		for (auto l : lands)
		{
			if (l->getTileAt(nowLand))
			{
				price = l->getProperty("level2BuildCost").asInt();
			}
		}
	}
	else if (gLand == level2_land_GID)
	{
		upgradeLand = ((String*)ngContent->objectForKey("upgradeLand3"))->getCString();

		for (auto l : lands)
		{
			if (l->getTileAt(nowLand))
			{
				price = l->getProperty("level3BuildCost").asInt();
			}
		}
	}
	else if (gLand == level3_land_GID)
	{
		// 輿仇厮葎恷互雫��涙俶起竃斤三崇
	}

	const char* payToUpgrade = ((String*)ngContent->objectForKey("payToUpgrade"))->getCString();
	const char* yuan = ((String*)ngContent->objectForKey("yuan"))->getCString();

	string s = nc + blank + comma + upgradeLand + "\n" +
		payToUpgrade + blank + to_string(price) + yuan;
	noticeL = Label::createWithSystemFont(s, "arial", 25);

	menuBoard->addChild(noticeL);
	noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
	noticeL->setTextColor(Color4B::BLACK);

	noticeMenu = Menu::create(yesM, noM, NULL);
	this->addChild(noticeMenu);
}

void GameMainLayer::myMenuYes()
{
	int n = 1;

	for (auto& p : players)
	{
		if (n == nowPlayerNumber)
		{
			int buildCost = 0;
			auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

			for (auto l : lands)
			{
				if (l->getTileAt(nowLand))
				{
					if (gLand == empty_land_GID)
					{
						buildCost = l->getProperty("level1BuildCost").asInt();
					}
					else if (gLand == level1_land_GID)
					{
						buildCost = l->getProperty("level2BuildCost").asInt();
					}
					else if (gLand == level2_land_GID)
					{
						buildCost = l->getProperty("level3BuildCost").asInt();
					}

					if (p.money >= buildCost)
					{
						if (gLand == empty_land_GID)
						{
							l->setTileGID(level1_land_GID, nowLand);
						}
						else if (gLand == level1_land_GID)
						{
							l->setTileGID(level2_land_GID, nowLand);
						}
						else if (gLand == level2_land_GID)
						{
							l->setTileGID(level3_land_GID, nowLand);
						}

						l->getTileAt(nowLand)->setColor(p.color);
						p.money -= buildCost;
						
						// 腺徨�塋�
						ParticleGalaxy* pg = ParticleGalaxy::create();

						switch (p.faceTo)
						{
						case faceForward::left:
							pg->setPosition(p.spritePosition.x, p.spritePosition.y + 30);
							break;
						case faceForward::down:
							pg->setPosition(p.spritePosition.x - 30, p.spritePosition.y);
							break;
						case faceForward::right:
							pg->setPosition(p.spritePosition.x, p.spritePosition.y - 30);
							break;
						case faceForward::up:
							pg->setPosition(p.spritePosition.x + 30, p.spritePosition.y);
							break;
						}
						pg->setStartColor(Color4F::Color4F(p.color.r / 255, p.color.g / 255, p.color.b / 255, 255));
						pg->setEndColor(Color4F::Color4F(0, 0, 0, 0));
						this->addChild(pg, 0, "particle");

						this->scheduleOnce(schedule_selector(GameMainLayer::removeParticle), 1.0f);

						// ここまでだ

						auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");
						const char* rmb = ((String*)ngContent->objectForKey("rmb"))->getCString();
						string blank = " ";
						string s = rmb + blank + to_string(p.money);

						((Label*)this->getChildByName(p.name + blank + "money"))->setString(s);

						this->cleanAndChange();
					}
					else
					{
						this->cleanMenu();

						// 暇汽中医夕頭
						menuBoard = Sprite::create("image/Popup.png");
						menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
						this->addChild(menuBoard);

						// 暇汽�哉袈�
						MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
							"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::cleanAndChange, this));
						okM->setPosition(0, -visibleSize.height / 5);

						const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
						Label* okL = Label::createWithSystemFont(okC, "arial", 20);
						okM->addChild(okL);
						okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
						okL->setTextColor(Color4B::BLACK);

						// 暇汽麼勣坪否
						const char* lackOfMoney = ((String*)ngContent->objectForKey("lackOfMoney"))->getCString();
						Label* noticeL = Label::createWithSystemFont(lackOfMoney, "arial", 25);

						menuBoard->addChild(noticeL);
						noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
						noticeL->setTextColor(Color4B::BLACK);

						noticeMenu = Menu::create(okM, NULL);
						this->addChild(noticeMenu);
					}

					break;
				}
			}

			break;
		}

		n++;
	}
}

void GameMainLayer::myMenuNo()
{
	this->cleanAndChange();
}

// 麿断議

void GameMainLayer::otherLand()
{
	string payName, earnName;
	faceForward nowF;
	int n = 1;

	for (auto p : players)
	{
		if (n == nowPlayerNumber)
		{
			payName = p.name;
			nowF = p.faceTo;
		}
		if (p.color == sLand->getColor())
		{
			earnName = p.name;
		}

		n++;
	}

	// 暇汽中医夕頭
	menuBoard = Sprite::create("image/Popup.png");
	menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(menuBoard);

	// 忖隅断
	auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

	// 戻幣崇坪否
	Label* noticeL;
	const char* belongLand = ((String*)ngContent->objectForKey("belongLand"))->getCString();
	string blank = " ";
	const char* earnC = ((String*)ngContent->objectForKey(earnName))->getCString();
	const char* comma = ((String*)ngContent->objectForKey("comma"))->getCString();
	const char* payC = ((String*)ngContent->objectForKey(payName))->getCString();
	const char* payLand = ((String*)ngContent->objectForKey("payLand"))->getCString();

	int price, rateTimes;
	int rateTimes1 = 1;
	int rateTimes2 = 1;
	int rate = 0;

	for (auto l : lands)
	{
		if (l->getTileAt(nowLand))
		{
			// �狒旃佻猴�螺社議仇��2翠+5%��3翠+10%��4翠+20%��5翠+50%,6翠+100%

			if (nowF == faceForward::down || nowF == faceForward::up)
			{
				// �鯢�
				for (;;)
				{
					if (l->getTileAt(Vec2(nowLand.x, nowLand.y - rateTimes1)))
					{
						if (l->getTileAt(Vec2(nowLand.x, nowLand.y - rateTimes1))->getColor() ==
							l->getTileAt(nowLand)->getColor())
						{
							rateTimes1++;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}

				// �鯱�
				for (;;)
				{
					if (l->getTileAt(Vec2(nowLand.x, nowLand.y + rateTimes2)))
					{
						if (l->getTileAt(Vec2(nowLand.x, nowLand.y + rateTimes2))->getColor() ==
							l->getTileAt(nowLand)->getColor())
						{
							rateTimes2++;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
			else if (nowF == faceForward::right || nowF == faceForward::left)
			{
				// �鰈�
				for (;;)
				{
					if (l->getTileAt(Vec2(nowLand.x - rateTimes1, nowLand.y)))
					{
						if (l->getTileAt(Vec2(nowLand.x - rateTimes1, nowLand.y))->getColor() ==
							l->getTileAt(nowLand)->getColor())
						{
							rateTimes1++;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}

				// �鰉�
				for (;;)
				{
					if (l->getTileAt(Vec2(nowLand.x + rateTimes2, nowLand.y)))
					{
						if (l->getTileAt(Vec2(nowLand.x + rateTimes2, nowLand.y))->getColor() ==
							l->getTileAt(nowLand)->getColor())
						{
							rateTimes2++;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}

			// 功象音揖議媼仇方楚��辺函音揖議継喘
			rateTimes = rateTimes1 + rateTimes2;
			switch (rateTimes)
			{
			case 2:
				rate = 0;
				break;
			case 3:
				rate = 5;
				break;
			case 4:
				rate = 10;
				break;
			case 5:
				rate = 20;
				break;
			case 6:
				rate = 50;
				break;
			case 7:
				rate = 100;
				break;
			}

			if (gLand == empty_land_GID)
			{
				price = l->getProperty("emptyValue").asInt();
			}
			else if (gLand == level1_land_GID)
			{
				price = l->getProperty("level1Value").asInt();
			}
			else if (gLand == level2_land_GID)
			{
				price = l->getProperty("level2Value").asInt();
			}
			else if (gLand == level3_land_GID)
			{
				price = l->getProperty("level3Value").asInt();
			}

			price += price*(rate / 100);
		}
	}

	const char* yuan = ((String*)ngContent->objectForKey("yuan"))->getCString();

	string s;
	if (rate == 0)
	{
		s = belongLand + blank + earnC + comma + payC + "\n" +
			payLand + blank + to_string(price) + yuan;
	}
	else
	{
		s = belongLand + blank + earnC + comma + payC + "\n" +
			payLand + blank + to_string(price) + yuan + "(+" + to_string(rate) + "%)";
	}

	noticeL = Label::createWithSystemFont(s, "arial", 25);

	menuBoard->addChild(noticeL);
	noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
	noticeL->setTextColor(Color4B::BLACK);

	// 暇汽�哉袈�
	MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
		"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::checkPayPlayerMoney, this, payName, earnName, price));
	okM->setPosition(0, -visibleSize.height / 5);

	const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
	Label* okL = Label::createWithSystemFont(okC, "arial", 20);
	okM->addChild(okL);
	okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
	okL->setTextColor(Color4B::BLACK);

	noticeMenu = Menu::create(okM, NULL);
	this->addChild(noticeMenu);
}

void GameMainLayer::checkPayPlayerMoney(string payName, string earnName, int price)
{
	// 羽薦殊臥

	for (auto& p : players)
	{
		if (p.name == payName)
		{
			if (p.money >= price)
			{
				this->otherMenuClose(payName, earnName, price);
			}
			else
			{
				bool sellFlag = false;
				bool thisTimeFlag = false;
				int sellNumber = 0;

				int sellMoney = 0;
				int wholeSellMoney = 0;

				int landNumbers = 0;

				while (p.money < price)
				{
					int endX = 24;
					int endY = 21;

					sellMoney = 0;
					landNumbers = 0;
					thisTimeFlag = false;

					for (auto l : lands)
					{
						++landNumbers;

						for (int y = 0; y < endY; ++y)
						{
							for (int x = 0; x < endX; ++x)
							{
								if (l->getTileAt(Vec2(x, y)))
								{
									if (l->getTileAt(Vec2(x, y))->getColor() == p.color)
									{
										sellFlag = true;
										thisTimeFlag = true;
										++sellNumber;

										if (l->getTileGIDAt(Vec2(x, y)) == empty_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt();
										}
										else if (l->getTileGIDAt(Vec2(x, y)) == level1_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt()
												+ l->getProperty("level1BuildCost").asInt();
										}
										else if (l->getTileGIDAt(Vec2(x, y)) == level2_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt()
												+ l->getProperty("level1BuildCost").asInt()
												+ l->getProperty("level2BuildCost").asInt();
										}
										else if (l->getTileGIDAt(Vec2(x, y)) == level3_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt()
												+ l->getProperty("level1BuildCost").asInt()
												+ l->getProperty("level2BuildCost").asInt()
												+ l->getProperty("level3BuildCost").asInt();
										}

										// 委輿仇指拷云寔
										l->setTileGID(empty_land_GID, Vec2(x, y));
										l->getTileAt(Vec2(x, y))->setColor(Color3B::WHITE);

										break;
									}
								}
							}

							if (thisTimeFlag == true)
							{
								break;
							}
						}

						if (thisTimeFlag == true)
						{
							break;
						}
					}

					wholeSellMoney += sellMoney;
					p.money += sellMoney;

					if (thisTimeFlag == false && landNumbers == lands.size() && sellMoney == 0)
					{
						break;
					}
				}

				if (sellFlag == true)
				{
					auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");
					const char* selll1 = ((String*)ngContent->objectForKey("sellLand1"))->getCString();
					const char* selll2 = ((String*)ngContent->objectForKey("sellLand2"))->getCString();
					const char* selll3 = ((String*)ngContent->objectForKey("sellLand3"))->getCString();
					const char* selll4 = ((String*)ngContent->objectForKey("sellLand4"))->getCString();
					const char* yuan = ((String*)ngContent->objectForKey("yuan"))->getCString();
					const char* comma = ((String*)ngContent->objectForKey("comma"))->getCString();
					string blank = "";
					Sprite* s = Sprite::create("image/Popup.png");
					s->setPosition(visibleSize / 2);

					string str = blank + selll1  + "\n"
						+ selll2 + to_string(sellNumber) + selll3 + to_string(wholeSellMoney) + yuan + comma + "\n"
						+ selll4 + to_string(p.money) + yuan;
					Label* la = Label::create(str, "arial", 25);
					la->setTextColor(Color4B::BLACK);
					la->setPosition(s->getContentSize().width / 2,
						s->getContentSize().height * 3 / 4);
					s->addChild(la);

					Button* btn = Button::create("image/GreenNormal.png", "image/GreenPressed.png");
					btn->setPosition(Vec2(s->getContentSize().width / 2,
						s->getContentSize().height / 4 - btn->getContentSize().height));
					btn->addTouchEventListener(CC_CALLBACK_2(GameMainLayer::checkToClose, this, payName, earnName, price));
					btn->setPressedActionEnabled(true);

					Label* ok = Label::create("OK", "arial", 20);
					ok->setColor(Color3B::BLACK);
					ok->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
					btn->addChild(ok);

					s->addChild(btn);
										
					this->addChild(s, 50, "RUA");
				}
				else
				{
					this->otherMenuClose(payName, earnName, price);
				}
			}
			
			break;
		}
	}
}

void GameMainLayer::checkToClose(Ref* pSender, Widget::TouchEventType type, string payName, string earnName, int price)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		break;
	case Widget::TouchEventType::MOVED:
		break;
	case Widget::TouchEventType::ENDED:
	{
		auto a = this->getChildByName("RUA");
		a->removeFromParentAndCleanup(true);

		this->otherMenuClose(payName, earnName, price);
	}
	case Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void GameMainLayer::otherMenuClose(string payName, string earnName, int price)
{
	// 孀竃斤哘議曾倖螺社��序佩署熱住叟
	for (auto& p : players)
	{
		for (auto& e : players)
		{
			if (p.name == payName && e.name == earnName)
			{
				p.money -= price;
				e.money += price;

				auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");
				const char* rmb = ((String*)ngContent->objectForKey("rmb"))->getCString();
				string blank = " ";
				string sp = rmb + blank + to_string(p.money);
				string se = rmb + blank + to_string(e.money);

				((Label*)this->getChildByName(payName + blank + "money"))->setString(sp);
				((Label*)this->getChildByName(earnName + blank + "money"))->setString(se);

				break;
			}
		}
	}

	this->cleanAndChange();
}

// 卿熱阻

void GameMainLayer::checkPayPlayerMoneyLo(string payName, int loss)
{
	// 羽薦殊臥

	for (auto& p : players)
	{
		if (p.name == payName)
		{
			if (p.money >= loss)
			{
				this->payLoss(payName, loss);
			}
			else
			{
				bool sellFlag = false;
				bool thisTimeFlag = false;
				int sellNumber = 0;

				int sellMoney = 0;
				int wholeSellMoney = 0;

				int landNumbers = 0;

				while (p.money < loss)
				{
					int endX = 24;
					int endY = 21;

					sellMoney = 0;
					landNumbers = 0;
					thisTimeFlag = false;

					for (auto l : lands)
					{
						++landNumbers;

						for (int y = 0; y < endY; ++y)
						{
							for (int x = 0; x < endX; ++x)
							{
								if (l->getTileAt(Vec2(x, y)))
								{
									if (l->getTileAt(Vec2(x, y))->getColor() == p.color)
									{
										sellFlag = true;
										++sellNumber;
										thisTimeFlag = true;

										if (l->getTileGIDAt(Vec2(x, y)) == empty_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt();
										}
										else if (l->getTileGIDAt(Vec2(x, y)) == level1_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt()
												+ l->getProperty("level1BuildCost").asInt();
										}
										else if (l->getTileGIDAt(Vec2(x, y)) == level2_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt()
												+ l->getProperty("level1BuildCost").asInt()
												+ l->getProperty("level2BuildCost").asInt();
										}
										else if (l->getTileGIDAt(Vec2(x, y)) == level3_land_GID)
										{
											sellMoney = l->getProperty("emptyBuildCost").asInt()
												+ l->getProperty("level1BuildCost").asInt()
												+ l->getProperty("level2BuildCost").asInt()
												+ l->getProperty("level3BuildCost").asInt();
										}

										// 委輿仇指拷云寔
										l->setTileGID(empty_land_GID, Vec2(x, y));
										l->getTileAt(Vec2(x, y))->setColor(Color3B::WHITE);

										break;
									}
								}
							}

							if (thisTimeFlag == true)
							{
								break;
							}
						}

						if (thisTimeFlag == true)
						{
							break;
						}
					}

					wholeSellMoney += sellMoney;
					p.money += sellMoney;

					if (thisTimeFlag == false && landNumbers == lands.size() && sellMoney == 0)
					{
						break;
					}
				}

				if (sellFlag == true)
				{
					auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");
					const char* selll1 = ((String*)ngContent->objectForKey("sellLand1"))->getCString();
					const char* selll2 = ((String*)ngContent->objectForKey("sellLand2"))->getCString();
					const char* selll3 = ((String*)ngContent->objectForKey("sellLand3"))->getCString();
					const char* selll4 = ((String*)ngContent->objectForKey("sellLand4"))->getCString();
					const char* yuan = ((String*)ngContent->objectForKey("yuan"))->getCString();
					const char* comma = ((String*)ngContent->objectForKey("comma"))->getCString();
					string blank = "";
					Sprite* s = Sprite::create("image/Popup.png");
					s->setPosition(visibleSize / 2);

					string str = blank + selll1 + "\n"
						+ selll2 + to_string(sellNumber) + selll3 + to_string(wholeSellMoney) + yuan + comma + "\n"
						+ selll4 + to_string(p.money) + yuan;
					Label* la = Label::create(str, "arial", 25);
					la->setTextColor(Color4B::BLACK);
					la->setPosition(s->getContentSize().width / 2,
						s->getContentSize().height * 3 / 4);
					s->addChild(la);

					Button* btn = Button::create("image/GreenNormal.png", "image/GreenPressed.png");
					btn->setPosition(Vec2(s->getContentSize().width / 2,
						s->getContentSize().height / 4 - btn->getContentSize().height));
					btn->addTouchEventListener(CC_CALLBACK_2(GameMainLayer::checkToCloseLo, this, payName, loss));
					btn->setPressedActionEnabled(true);

					Label* ok = Label::create("OK", "arial", 20);
					ok->setColor(Color3B::BLACK);
					ok->setPosition(btn->getContentSize().width / 2, btn->getContentSize().height / 2);
					btn->addChild(ok);

					s->addChild(btn);

					this->addChild(s, 50, "RUA");
				}
				else
				{
					this->payLoss(payName, loss);
				}
			}

			break;
		}
	}
}

void GameMainLayer::checkToCloseLo(Ref* pSender, Widget::TouchEventType type, string payName, int loss)
{
	switch (type)
	{
	case Widget::TouchEventType::BEGAN:
		break;
	case Widget::TouchEventType::MOVED:
		break;
	case Widget::TouchEventType::ENDED:
	{
		auto a = this->getChildByName("RUA");
		a->removeFromParentAndCleanup(true);

		this->payLoss(payName, loss);
	}
	case Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void GameMainLayer::payLoss(string payName, int loss)
{
	for (auto& p : players)
	{
		if (p.name == payName)
		{
			auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");
			const char* rmb = ((String*)ngContent->objectForKey("rmb"))->getCString();
			string blank = " ";
			p.money -= loss;

			string sp = rmb + blank + to_string(p.money);

			((Label*)this->getChildByName(payName + blank + "money"))->setString(sp);

			break;
		}
	}

	this->cleanAndChange();
}

// 賠茅�犢愃�方

void GameMainLayer::cleanMenu()
{
	noticeMenu->removeFromParentAndCleanup(true);
	menuBoard->removeFromParentAndCleanup(true);
}

void GameMainLayer::changePlayer()
{
	int n = 1;

	for (auto& p : players)
	{
		if (n == nowPlayerNumber)
		{
			p.isGoing = false;

			break;
		}

		n++;
	}

	// 泌惚螺社署熱弌噐0��夸餅竃嗄老
	n = 0;
	bool flag = false;

	for (auto& p : players)
	{
		if (p.money < 0)
		{
			// 暇汽中医夕頭
			menuBoard = Sprite::create("image/Popup.png");
			menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
			this->addChild(menuBoard);

			// 忖隅断
			auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

			// 戻幣崇坪否
			Label* noticeL;
			const char* belongLand = ((String*)ngContent->objectForKey("belongLand"))->getCString();
			const char* eliminate = ((String*)ngContent->objectForKey("eliminate"))->getCString();
			string str = "";
			string s = "";

			if (n == 0)
			{
				const char* player = ((String*)ngContent->objectForKey("player1"))->getCString();
				s = player + str + eliminate;
			}
			else if (n == 1)
			{
				const char* player = ((String*)ngContent->objectForKey("player2"))->getCString();
				s = player + str + eliminate;
			}
			else if (n == 2)
			{
				const char* player = ((String*)ngContent->objectForKey("player3"))->getCString();
				s = player + str + eliminate;
			}
			else if (n == 3)
			{
				const char* player = ((String*)ngContent->objectForKey("player4"))->getCString();
				s = player + str + eliminate;
			}

			noticeL = Label::createWithSystemFont(s, "arial", 25);

			menuBoard->addChild(noticeL);
			noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
			noticeL->setTextColor(Color4B::BLACK);

			// 暇汽�哉袈�
			MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
				"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::removePlayer, this, n));
			okM->setPosition(0, -visibleSize.height / 5);

			const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
			Label* okL = Label::createWithSystemFont(okC, "arial", 20);
			okM->addChild(okL);
			okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
			okL->setTextColor(Color4B::BLACK);

			noticeMenu = Menu::create(okM, NULL);
			this->addChild(noticeMenu);

			flag = true;
			break;
		}
		n++;
	}

	// 泌惚音贋壓猛稔螺社��屎械態算
	if (flag == false)
	{
		nowPlayerNumber++;
		if (nowPlayerNumber > players.size())
		{
			nowPlayerNumber = 1;

			// 俳算指栽方
			rounds++;
			((Label*)this->getChildByName("roundsL"))->setString(to_string(rounds));
		}
	}
}

void GameMainLayer::removePlayer(int number)
{
	int n = 0;
	for (auto& p : players)
	{
		if (n == number)
		{
			this->removeChildByName("player" + to_string(n + 1), true);

			players.erase(players.begin() + number);

			break;
		}
		n++;
	}

	this->cleanMenu();

	// 登僅頁倦峪複匯兆螺社��飛頁��夸嗄老潤崩
	bool flag = false;

	if (players.size() == 1)
	{
		// 慧咄赤
		SimpleAudioEngine::getInstance()->playEffect("music/win.mp3", false);
		SimpleAudioEngine::getInstance()->playBackgroundMusic("music/end.mp3", true);

		// 暇汽中医夕頭
		menuBoard = Sprite::create("image/Popup.png");
		menuBoard->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		this->addChild(menuBoard);

		// 忖隅断
		auto ngContent = Dictionary::createWithContentsOfFile("XML/NewGame.xml");

		// 戻幣崇坪否
		Label* noticeL;
		const char* belongLand = ((String*)ngContent->objectForKey("belongLand"))->getCString();
		const char* win = ((String*)ngContent->objectForKey("win"))->getCString();
		string str = "";
		string s = "";

		if (players[0].name == "player1")
		{
			const char* player = ((String*)ngContent->objectForKey("player1"))->getCString();
			s = player + str + win;
		}
		else if (players[0].name == "player2")
		{
			const char* player = ((String*)ngContent->objectForKey("player2"))->getCString();
			s = player + str + win;
		}
		else if (players[0].name == "player3")
		{
			const char* player = ((String*)ngContent->objectForKey("player3"))->getCString();
			s = player + str + win;
		}
		else if (players[0].name == "player4")
		{
			const char* player = ((String*)ngContent->objectForKey("player4"))->getCString();
			s = player + str + win;
		}

		noticeL = Label::createWithSystemFont(s, "arial", 25);

		menuBoard->addChild(noticeL);
		noticeL->setPosition(menuBoard->getContentSize().width / 2, menuBoard->getContentSize().height * 3 / 4);
		noticeL->setTextColor(Color4B::BLACK);

		// 暇汽�哉袈�
		MenuItem* okM = MenuItemImage::create("image/OrangeNormal.png",
			"image/OrangePressed.png", CC_CALLBACK_0(GameMainLayer::endGame, this));
		okM->setPosition(0, -visibleSize.height / 5);

		const char* okC = ((String*)ngContent->objectForKey("ok"))->getCString();
		Label* okL = Label::createWithSystemFont(okC, "arial", 20);
		okM->addChild(okL);
		okL->setPosition(okM->getContentSize().width / 2, okM->getContentSize().height / 2);
		okL->setTextColor(Color4B::BLACK);

		noticeMenu = Menu::create(okM, NULL);
		this->addChild(noticeMenu);

		flag = true;
	}

	if (flag == false)
	{
		nowPlayerNumber++;
		if (nowPlayerNumber > players.size())
		{
			nowPlayerNumber = 1;

			// 俳算指栽方
			rounds++;
			((Label*)this->getChildByName("roundsL"))->setString(to_string(rounds));
		}
	}
}

void GameMainLayer::cleanAndChange()
{
	this->cleanMenu();
	this->changePlayer();
}

void GameMainLayer::endGame()
{
	TitleScene* ts = TitleScene::create();
	Director::getInstance()->pushScene(ts);
}