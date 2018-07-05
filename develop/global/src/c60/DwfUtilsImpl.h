//
// Created by den on 05.07.18.
//

#pragma once

#include "dgn/DwfUtils.hpp"

/**реализация синглтона утилит для вызова из Java**/
class DwfUtilsImpl : public dgn::DwfUtils{

		/**получить синглтон утилит */
		static std::shared_ptr<DwfUtils> getInstance(){
			static DwfUtilsImpl instance;
			return instance;
		}

		/**выолнить импотр даных из потока dwf и передать их в обработчик handler */
		void doImport(const std::shared_ptr<InputStream> & dwf, const std::shared_ptr<DwfStreamHandler> & handler) override;
};


