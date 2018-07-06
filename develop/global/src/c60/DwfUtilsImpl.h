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

		/**реализация синглтона утилит для вызова из Java**/
		class DwfUtilsImpl : public dgn::DwfUtils {

				/**выолнить импотр даных из потока dwf и передать их в обработчик handler */
				void
				doImport(const std::shared_ptr<dgn::InputStream> &dwf, const std::shared_ptr<dgn::DwfStreamHandler> &handler) override;
		};


}