//
// Created by den on 05.07.18.
//

#pragma once

#include <iostream>
#include <string>

#include "dgn/DwfUtils.hpp"
#include <dgn/SemanticValue.hpp>
#include "dgn/DwfLayerStructureStreamHandler.hpp"
#include "dgn/DwfLayersFillStreamHandler.hpp"

#include "dwfcore/File.h"
#include "dwfcore/String.h"
#include "dwfcore/STL.h"
#include "dwfcore/UnzippingInputStream.h"

#include "dwf/package/Manifest.h"
#include "dwf/package/ContentManager.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/EModelSection.h"
#include "dwf/package/reader/PackageReader.h"

#include "dwf/w3dtk/BStream.h"
#include "dwf/w3dtk/BOpcodeShell.h"

using namespace DWFCore;
using namespace DWFToolkit;


namespace c60 {
		/**вектор с семантикой узлов дерева**/
		typedef std::vector<dgn::SemanticValue> OBJ_SEMANTIC;

		class SectionDWF;
		class CurrentSemantic;

		/**семантика текущего уровня**/
		//class CurrentSemantic{
		//public:
		//		std::map<std::wstring, std::wstring> items;
				/**семантика родителя**/
		//		CurrentSemantic *parent = NULL;

		//		CurrentSemantic (void){};

		//		vector<wstring> getValues(){
		//			vector<wstring> result;
		//			for (auto pos = this->items.begin(); pos != this->items.end(); ++pos) {
		//				result.push_back(pos->second);
		//			}

		//			return result;
		//		}

		//		void SetSemantic(std::map<std::wstring, std::wstring> fields, OBJ_SEMANTIC semantic){
		//			for (auto pos = fields.begin(); pos != fields.end(); ++pos) {
		//				this->items[pos->first] = pos->second;
		//			}

		//			for (auto const &semv : semantic){
		//				if (this->items[semv.propName] == L"")
		//					this->items[semv.propName] = semv.propValue;
		//				else
		//					this->items[semv.propName] += L" $$$ " + semv.propValue;
		//			}
		//		};
		//};

		/**слой для вызова метода doImportNextLayer**/
		class LayerDWF{
		private:
				CurrentSemantic *_semantic = NULL;
		public:
				/**Секция которой принадлежит слой**/
				//SectionDWF *section;
				/**семантика объектов слоя**/
				std::map<std::wstring, OBJ_SEMANTIC> semantics;
				//**Поток с деревом геометрии**//
				//DWFCore::DWFInputStream *pW3DStream;
				/**название слоя**/
				wstring name;
				/**название колонок**/
				//std::vector<wstring> fields;
				std::map<std::wstring, std::wstring> fields;
				/**Глубина в структуре узлов, на которой находится нижняя семантика**/
				//int32_t bottomLevel = 0;
				/**Глубина в структуре узлов, на которой находится семантика**/
				//int32_t semanticLevel = -1;
				//**Позиция начала слоя**//
				//int begin = -1;
				//**Позиция конца слоя**/
				//int end = -1;
				/**Количество объектов**/
				//int32_t countNode;
				/*Глубина в структуре узлов, на которой находится геометрия нового объекта*/
				int32_t geomLevel = -1;

				LayerDWF (void){};

				//bool GetSemantic(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
				bool ReadSemantic(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
				//void SetFields(DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
				//void SetBottomLevel(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
				//bool SetFields();
				void clearValue(){
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						pos->second = L"";
					}
				}
				void setValues(OBJ_SEMANTIC semantic){
					for (auto const &semv : semantic){
						if (this->fields[semv.propName] == L"")
							this->fields[semv.propName] = semv.propValue;
						//else
						//	this->fields[semv.propName] += L"\\" + semv.propValue;
					}
				}
				vector<wstring> getValues(){
					vector<wstring> result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back(pos->second);
					}

					return result;
					//return _semantic->getValues();
				}

				vector<wstring> getFields(){
					std::vector<wstring> result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back(pos->first);
					}

					return result;
				}

				OBJ_SEMANTIC getOBJ_SEMANTIC(){
					OBJ_SEMANTIC result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back((dgn::SemanticValue(pos->first, pos->second)));
					}

					return result;
				}

				/*
				void setCurrentSemantic(OBJ_SEMANTIC semantic){
					CurrentSemantic *ob = new CurrentSemantic();
					ob->parent = this->_semantic;
					if (ob->parent == NULL)
						ob->SetSemantic(this->fields, semantic);
					else
						ob->SetSemantic(ob->parent->items, semantic);
					this->_semantic = ob;
				}

				void setTopSemantic(){
					CurrentSemantic *ob = this->_semantic;
					if (ob) {
						this->_semantic = ob->parent;
					}
				}
				 */
		};

		/**секция файла DWF**/
		class SectionDWF{
		public:
				//**семантика всех узлов секции**//
				//std::map<std::wstring, OBJ_SEMANTIC> semantics;
				//**потоки с геометрией**//
				//std::vector<std::shared_ptr<DWFInputStream>> pW3DStreams;
				/**Текущий поток с деревом геометрии**/
				//DWFCore::DWFInputStream *pW3DStream;
				/**Структура с семантикой объектов**/
				//DWFObjectDefinition *pDef;


				/**Глубина в структуре узлов, на которой находятся слои**/
				int32_t levelLayers = -1;
				/**слои секции**/
				std::map<std::wstring, LayerDWF> layers;
				/**Текущий обрабатываемый слой текущей DWFSection**/
				LayerDWF *layer;


				//bool SetLevelLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler);
				bool SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler);
				//bool SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler);
				//void AddSemantic(DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
		};


		/**реализация синглтона утилит для вызова из Java**/
		class DwfUtilsImpl : public dgn::DwfUtils {
		protected:
				/**Открытый на чтение файл DWF**/
				DWFToolkit::DWFPackageReader oReader;
				/**Содержит информацию о файле DWF**/
				DWFToolkit::DWFPackageReader::tPackageInfo tInfo;

		public:
				/**Из какого файла читать**/
				const std::wstring dwfFile;
				/**Глубина в структуре узлов, на которой находятся слои**/
				const int32_t layerDepth;
				/**секции файла DWF**/
				std::vector<SectionDWF> sections;
				/**Текущая секция**/
				SectionDWF *section;
				/**слои для вызова метода doImportNextLayer**/
				//std::vector<LayerDWF> layers;
				/**Текущий обрабатываемый слой текущей DWFSection**/
				//LayerDWF *layer;
				/**Количество слоёв**/
				//int32_t countLayer;
				/**Количество объектов**/
				//int32_t countNode;

				DwfUtilsImpl(const std::wstring & dwfFile, int32_t layerDepth);
				//virtual ~DwfUtilsImpl ();


				/*
				 *
				 */
				void readTreeStructure(const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler) override;
				void readAllLayers(const std::shared_ptr<dgn::DwfLayersFillStreamHandler> &handler) override;

				static DWFString toDwfString(std::wstring str){
					DWFString name;
					name.append(str.c_str());
					return name;
				}

				static std::wstring toStdString(const DWFString &str){
					return std::wstring((const wchar_t *)str, str.chars());
				}
		};


}