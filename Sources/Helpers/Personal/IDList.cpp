#include "Helpers/IDList.hpp"
#include "Helpers/Game.hpp"
#include "Helpers/Player.hpp"
#include "Helpers/Inventory.hpp"
#include "Helpers/Save.hpp"
#include "Helpers/NPC.hpp"
#include "RegionCodes.hpp"

#define RANGE(X, START, END)	((X & 0xFFFF) >= START && (X & 0xFFFF) <= END)
#define IS(X, ADDR)				((X & 0xFFFF) == ADDR)

namespace CTRPluginFramework {
	Range KeyRange::sRange;

	void ValidKeyboardCheck(Keyboard& keyboard, KeyboardEvent& event) {
		std::string& input = keyboard.GetInput();	
		u16 ID = StringToHex<u16>(input, 0xFFFF);
		if(!IDList::ValidID((ID & 0xFFFF), KeyRange::sRange.start, KeyRange::sRange.end)) {
			keyboard.SetError(Color::Red << Language->Get("INVALID_ID"));
			return;
		}
	}

	void ItemChange(Keyboard& keyboard, KeyboardEvent& event) {
		std::string& input = keyboard.GetInput();	
		Item ID = (Item)StringToHex<u32>(input, 0xFFFF);
		if(!IDList::ItemValid(ID)) {
			keyboard.SetError(Color::Red << "Invalid ID!");
			return;
		}
	}

	void TextItemChange(Keyboard& keyboard, KeyboardEvent& event) {
		std::string& input = keyboard.GetInput();
		Item ID = (Item)StringToHex<u32>(input, 0xFFFF);

		if(!IDList::ItemValid(ID, false)) {
			keyboard.GetMessage() = "";
			keyboard.SetError(Color::Red << Language->Get("INVALID_ID"));
			return;
		}

		keyboard.GetMessage() = ItemIDSearch(ID);
	}

	bool IDList::IsHalfAcre(u8 acreID) {
		if(RANGE(acreID, 0x9E, 0xA3) || IS(acreID, 0xA8))
			return true;

		return false;
	}

	bool IDList::RoomValid(u8 roomID) {
		//if villager not exists room warp not work
		//if no exhibtion house those crash: 0x92 - 0x97

		if(!IDList::ValidID(roomID, 0, 0xA4))
			return false;

		if(IDList::ValidID(roomID, 0x27, 0x29) || IDList::ValidID(roomID, 0x5D, 0x60) || IDList::ValidID(roomID, 0x98, 0x9C) || IDList::ValidID(roomID, 0x69, 0x8D))
			return false;
		
		switch(roomID) {
			case 0x62: 
			case 0x65: 
			case 0x66: 
				return false;
		}
		
		return true;
	}
//If menu is valid
	bool IDList::MenuValid(u8 MenuID) {
		if(RANGE(MenuID, 5, 7) || RANGE(MenuID, 0x2E, 0x43) || RANGE(MenuID, 0x47, 0x48) || RANGE(MenuID, 0x5F, 0x60) || MenuID == 0x65 || 
			RANGE(MenuID, 0x6A, 0x6C) || RANGE(MenuID, 0x70, 0x71) || RANGE(MenuID, 0x79, 0x7A) || MenuID == 0x87 || MenuID == 0x89)
			return 1;

		return 0;
	}
//if building is valid
	bool IDList::BuildingValid(u8 buildingID) {
		if(buildingID > 0x4B && buildingID < 0x69 || buildingID > 0x6A && buildingID < 0x81 || buildingID > 0x8F && buildingID < 0xFC) 
			return true;
		
		return false;
	}
//if emotion is valid
	bool IDList::EmotionValid(u8 emotionID) {
		return (emotionID > 0 && emotionID < 0x41);
	}
//If snake is valid
	bool IDList::SnakeValid(u16 snakeID) {
		if(snakeID > 0 && snakeID < 0x276) {
			switch(snakeID) {
				default: return true;
				case 0xF7: 
				case 0xFA:
				case 0xFB:
				case 0xFC:
				case 0xFD:
				case 0x124:
				case 0x125:
					return false;
			}
		}
		
		return false;
	}
//If animation is valid
	bool IDList::AnimationValid(u8 animID) {
		if(animID > 0 && animID < 0xEB) {
			if(IsIndoorsBool) {
				switch(animID) {
				//those would be fixed but they crash others indoors :/
					case 0x5F:
					case 0x7E:
					case 0x87:
				//those teleport and i am not sure how to prevent it
					case 0x33:
					case 0xD1:
					case 0xD2:
					case 0xD3:
					case 0xD7:
					case 0xD8: 
						return false;
				}
			}
			
			if(!GameHelper::IsInRoom(0)) {
			//stuff for kappns boat
				switch(animID) {
					case 0xBB:
					case 0xBC:
					case 0xBE:
					case 0xBF: 
						return false;
				}
			}
			return true;
		}
		
		return false;
	}
//If music is valid
	bool IDList::MusicValid(u16 musicID) {
		if(musicID < 0xFFF) {
			if(musicID > 0xDB && musicID < 0x10B) 
				return false;
			
			return true;
		}
		
		return false;
	}

	bool FlagValid(u16 flagID, bool IsDropped) {
	//If player is indoors and drops item
		if(IsIndoorsBool && IsDropped) {
			if(flagID >= 0x8000)
				return false;
		}
		return true;
	}
//If item is valid
    bool IDList::ItemValid(Item itemID, bool IsDropped) {
        (void)ItemID;
        (void)IsDropped;
        return true;
    }

//If tool is valid
	bool IDList::ToolsValid(Item toolsID) {
		return (toolsID.ID == 0x2001 || (toolsID.ID == 0x3729) || (toolsID.ID > 0x334B && toolsID.ID < 0x33A3));
	}

//Get Building Name
	std::string IDList::GetBuildingName(u8 ID) {
		if(ID >= 8 && ID <= 0x11) {
			ACNL_VillagerData *villager = NPC::GetSaveData();
			if(!villager)
				return Utils::Format("NPC %d", ID - 8);

			u16 VID = villager->Villager[ID - 8].Mini1.VillagerID;
			if(VID == 0xFFFF)
				return "NPC -Empty-";

			return "NPC " + NPC::GetNName(VID);
		}

		for(const ID_Data& buildings : Buildings) {
			if(buildings.ID == ID) {
				return std::string(buildings.Name);
			}
		}

		return Language->Get("INVALID");
	}
//get country name
	std::string IDList::SetCountryName(u8 country) {
		for(const ID_Data& countrys : Countrys) {
			if(countrys.ID == country) {
				return std::string(countrys.Name);
			}
		}

		return Language->Get("INVALID");
	}
	
	bool IDList::GetSeedName(Item itemID, std::string& str) {
		for(int i = 0; i < ItemFileLenght; i++) {
			if(ItemList->ID[i] == itemID) {
				str = ItemList->Name[i];
				return true;
			}
		}
		return false;
	}

	/*std::string IDList::GetItemName(u16 ItemID) {
		static const u32 ItemFunc(0x2FEA78, 0, 0, 0, 0, 0, 0, 0); 

		u32 Stack[44];
		
		static FUNCTION func(ItemFunc);
		func.Call<void>(Stack, &ItemID, 0);

		std::string ItemName = "";
		Process::ReadString(Stack[1], ItemName, 0x20, StringFormat::Utf16);

		return ItemName.empty() ? "???" : ItemName;
	}*/ 

	

	//struct MsgBox {
	//	/*0x98D56C*/s8 answerCount = 0; //seems to tell how many answers (0 = 1, 1 = 2)
	//	/*0x98D56D*/s8 openingSound = 0; //seems to choose msg box opening sound
	//	/*0x98D56E*/s16 padding = 0; //never changes
	//	/*0x98D570*/s32 msg_stringName = -1;
	//	/*0x98D574*/s32 decline_stringName = -1;
	//	/*0x98D578*/s32 unknown3 = -1;
	//	/*0x98D57C*/s32 accept_stringName = -1;
	//	/*0x98D580*/s32 accept_soundID = -1;
	//	/*0x98D584*/s32 decline_soundID = -1;
	//	/*0x98D588*/s32 unknown6 = -1; //Either -1, -2, -3
	//	/*0x98D58C*/s32 unknown7 = 0; //Only Function 0x537D84 results get written to it
	//	/*0x98D590*/char* string_fileName = nullptr;
	//	/*0x98D594*/char* playerName = nullptr; //Seems to always be the player name
	//	/*0x98D598*/s32 unknown8 = 0; //never changes(?)
	//	/*0x98D59C*/s32 unknown9 = 0; //never changes(?)
	//	/*0x98D5A0*/s32* funcPointer = nullptr; //seems to only have Function 0x21ACE8
	//	/*0x98D5A4*/s8 unknown10 = 1; //Only gets 1 written to
	//	/*0x98D5A5*/s8 unknown11 = 1; //Only gets 1 or 0 written to
	//	/*0x98D5A6*/s8 unknown12 = 0; //Only gets 1 or 0 written to
	//	/*0x98D5A7*/s8 unknown13 = 0; //Only gets 1 written to
	//};

	//FUNCT(0x6D32D0).Call<void>(); //Clear MSG Box Struct (0x098D56C)

	//0x251B70(0x32DC48B8)

	/*

	void func(void) {
		MsgBox msgbox;
		msgbox.answerCount = 1;

		msgbox.accept_stringName = 0x180;
		msgbox.decline_stringName = 0x181;
		msgbox.msg_stringName = 0x177;

		msgbox.string_fileName = (char *)"SYS_2D_UI";

		msgbox.unknown10 = 1;
		msgbox.unknown11 = 1;
		msgbox.unknown12 = 0;

		msgbox.accept_soundID = 0x1000429;
	}

	*/

//Get Room Name
	std::string IDList::GetRoomName(u8 ID) {
		static Address RoomName(0x5B4BE4, 0x5B40FC, 0x5B3C2C, 0x5B3C2C, 0x5B351C, 0x5B351C, 0x5B31F0, 0x5B31F0); 

		if(ID <= 0xA5) 
			return Color::Green << (std::string)(RoomName.Call<char *>(ID));
		
		return Color::Red << Language->Get("INVALID");
	}
//If Invalid ID
	bool IDList::ValidID(u16 ID, u16 StardID, u16 EndID) {
		return (ID >= StardID && ID <= EndID);
	}	
}
