#include <Geode/Geode.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
using namespace geode::prelude;

template <class T>
class MixedValuesInput;

class $modify(CompressSetGroupIDLayer, SetGroupIDLayer) {
	static void onModify(auto& self) {
		if (!self.setHookPriorityPre("SetGroupIDLayer::init", Priority::Early)) {
			log::warn("Failed to set hook priority for SetGroupIDLayer::init");
		}
	}

	void onCompress(CCObject* sender) {
		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
		auto zOrderMenu = static_cast<CCMenu*>(btn->getParent());
		auto mainLayer = static_cast<CCLayer*>(zOrderMenu->getParent());
		auto self = static_cast<CompressSetGroupIDLayer*>(mainLayer->getParent());

		auto objArr = self->m_targetObjects;

		if (objArr && objArr->count() > 0) {
			if (!Mod::get()->getSettingValue<bool>("consider-z-layer")) {

				// Init z's ordered set
				std::set<int>posOrderSet;
				std::set<int>negOrderSet;
				for (auto objInArr : CCArrayExt<GameObject*>(objArr)) {
					if (objInArr->m_zOrder > 0)
						posOrderSet.insert(objInArr->m_zOrder);
					else if (objInArr->m_zOrder < 0)
						negOrderSet.insert(objInArr->m_zOrder);
				}


				// Mapping z's
				std::unordered_map<int, int> orderRemap;
				int posI = 1;
				for (int i : posOrderSet) {
					if (!orderRemap.contains(i)) {
						orderRemap[i] = posI;
						posI++;
					}
				}

				int negI = -1;
				for (auto it = negOrderSet.rbegin(); it != negOrderSet.rend(); ++it) {
					if (!orderRemap.contains(*it)) {
						orderRemap[*it] = negI;
						negI--;
					}
				}


				// Inserting new mapped z's to Objects
				for (auto objInArr : CCArrayExt<GameObject*>(objArr)) {
					objInArr->m_zOrder = orderRemap[objInArr->m_zOrder];
				}
			}
			else {
				std::unordered_map<ZLayer, CCArray*> objectsMap;
				for (auto objInArr : CCArrayExt<GameObject*>(objArr)) {
					auto zLayer = objInArr->m_zLayer;
					if (!objectsMap.contains(zLayer)) {
						CCArray* zLayerObjectsArr = CCArray::create();
						objectsMap[zLayer] = zLayerObjectsArr;
					}
					objectsMap[zLayer]->addObject(objInArr);
				}

				for (auto mapPair : objectsMap) {
					auto zLayerObjectsArr = mapPair.second;

					// Init z's ordered set
					std::set<int>posOrderSet;
					std::set<int>negOrderSet;
					for (auto objInArr : CCArrayExt<GameObject*>(zLayerObjectsArr)) {
						if (objInArr->m_zOrder > 0)
							posOrderSet.insert(objInArr->m_zOrder);
						else if (objInArr->m_zOrder < 0)
							negOrderSet.insert(objInArr->m_zOrder);
					}


					// Mapping z's
					std::unordered_map<int, int> orderRemap;
					int posI = 1;
					for (int i : posOrderSet) {
						if (!orderRemap.contains(i)) {
							orderRemap[i] = posI;
							posI++;
						}
					}

					int negI = -1;
					for (auto it = negOrderSet.rbegin(); it != negOrderSet.rend(); ++it) {
						if (!orderRemap.contains(*it)) {
							orderRemap[*it] = negI;
							negI--;
						}
					}


					// Inserting new mapped z's to Objects
					for (auto objInArr : CCArrayExt<GameObject*>(zLayerObjectsArr)) {
						objInArr->m_zOrder = orderRemap[objInArr->m_zOrder];
					}
				}
			}
		}
	}

	bool init(GameObject* obj, CCArray* objArr) {
		if (!SetGroupIDLayer::init(obj, objArr)) return false;
		if (!this->m_targetObjects) return true;
		if (this->m_targetObjects->count() == 0) return true;

		CCLayer* mainLayer = static_cast<CCLayer*>(this->getChildByID("main-layer"));
		if (!mainLayer) return true;

		CCMenu* zOrderMenu = static_cast<CCMenu*>(mainLayer->getChildByID("z-order-menu"));
		if (!zOrderMenu) return true;

		auto zOrderInput = static_cast<geode::TextInput*>(zOrderMenu->getChildByID("z-order-input"));

		ButtonSprite* compressBtnSprite = nullptr;
		compressBtnSprite = ButtonSprite::create("Compress", "goldFont.fnt", "GJ_button_05.png");

		// BetterEdit check
		auto unmixButton = static_cast<CCMenuItemSpriteExtra*>(zOrderMenu->getChildByID("hjfod.betteredit/unmix-button"));
		if (unmixButton && unmixButton->isVisible()) {
			compressBtnSprite->setScale(0.30f);

			CCMenuItemSpriteExtra* compressBtn = CCMenuItemSpriteExtra::create(compressBtnSprite, this, menu_selector(CompressSetGroupIDLayer::onCompress));
			zOrderMenu->addChild(compressBtn);
			compressBtn->setID("compress-button"_spr);

			auto bothButtonsWidth = compressBtn->getContentWidth() + unmixButton->getContentWidth();

			unmixButton->setPosition(
				zOrderMenu->getContentWidth() / 2.f - bothButtonsWidth / 2.f + unmixButton->getContentWidth() / 2.f - 2.f,
				unmixButton->getPositionY()
			);

			compressBtn->setPosition({
				unmixButton->getPositionX() + unmixButton->getContentWidth() / 2.f + compressBtn->getContentWidth() / 2.f + 2.f,
				unmixButton->getPositionY()
				});
			
		}
		else {
			compressBtnSprite->setScale(0.45f);
			CCMenuItemSpriteExtra* compressBtn = CCMenuItemSpriteExtra::create(compressBtnSprite, this, menu_selector(CompressSetGroupIDLayer::onCompress));
			zOrderMenu->addChild(compressBtn);
			compressBtn->setID("compress-button"_spr);
			compressBtn->setPosition({
				zOrderMenu->getContentWidth() / 2.f,
				zOrderInput->getPositionY() - zOrderInput->getContentHeight() / 2.f - compressBtn->getContentHeight() / 2.f
				});
		}
		
		return true;
	}
};
