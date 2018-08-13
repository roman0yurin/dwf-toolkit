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

		/**слой для вызова метода doImportNextLayer**/
		class LayerDWF{
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
				/**Глубина в структуре узлов, на которой находится геометрия**/
				int32_t bottomLevel = 0;
				/**Глубина в структуре узлов, на которой находится семантика**/
				int32_t semanticLevel = -1;
				//**Позиция начала слоя**//
				//int begin = -1;
				//**Позиция конца слоя**/
				//int end = -1;
				/**Количество объектов**/
				//int32_t countNode;

				LayerDWF (void){};

				//void SetFields(DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
				//void SetBottomLevel(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);
				bool SetFields();
				void clearValue(){
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						pos->second = L"";
					}
				}
				void setValues(OBJ_SEMANTIC semantic){
					for (auto const &semv : semantic){
						this->fields[semv.propName] = semv.propValue;
					}
				}
				vector<wstring> getValues(){
					vector<wstring> result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back(pos->second);
					}

					return result;
				}
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
				int32_t levelLayers = 0;
				/**слои секции**/
				std::map<std::wstring, LayerDWF> layers;
				/**Текущий обрабатываемый слой текущей DWFSection**/
				LayerDWF *layer;


				bool SetLevelLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler);
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
		};


}