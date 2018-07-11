//
// Created by den on 05.07.18.
//

#pragma once

#include "dgn/DwfUtils.hpp"
#include "dwfcore/STL.h"
#include <iostream>

#include "dwfcore/File.h"
#include "dwfcore/String.h"


#include "dwf/package/Manifest.h"
#include "dwf/package/ContentManager.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/EModelSection.h"
#include "dwf/package/reader/PackageReader.h"

#include "dwf/w3dtk/BStream.h"
#include "dwf/w3dtk/BOpcodeShell.h"

namespace c60 {
		//**вектор с семантикой узлов дерева**//
		typedef std::vector<dgn::SemanticValue> OBJ_SEMANTIC;

		/**реализация синглтона утилит для вызова из Java**/
		class DwfUtilsImpl : public dgn::DwfUtils {
		protected:
				/**Открытый на чтение файл DWF**/
				DWFToolkit::DWFPackageReader oReader;

				/**Содержит информацию о файле DWF**/
				DWFToolkit::DWFPackageReader::tPackageInfo tInfo;

				/**Содержит пакеты файла**/
				DWFToolkit::DWFManifest &rManifest;

				//**семантика всех узлов дерева**//
				std::map<std::wstring, OBJ_SEMANTIC> semantics;
		public:
				/**Из какого файла читать**/
				const std::wstring dwfFile;
				/**Глубина в структуре узлов, на которой находятся слои**/
				const int32_t layerDepth;

				DwfUtilsImpl(const std::wstring & dwfFile, int32_t layerDepth);

				void doImportNextLayer(const std::shared_ptr<dgn::DwfStreamHandler> &handler) override;

				/**
			 *Читает исключительно  структуру дерева (обрабатываются только методы openNode и closeNode)
			 *Причем не глубже указанного уровня.
			 */
				virtual void readTreeStructure(const std::shared_ptr<dgn::DwfStreamHandler> & handler) override;
		};


}