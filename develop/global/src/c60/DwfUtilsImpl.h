//
// Created by den on 05.07.18.
//

#pragma once

#include <iostream>
#include <string>
#include <time.h>

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
#include "JLogger.h"

using namespace DWFCore;
using namespace DWFToolkit;


namespace c60 {
		/**вектор с семантикой узлов дерева**/
		typedef std::vector<dgn::SemanticValue> OBJ_SEMANTIC;

		/**Перемножить матрицы 4x4 m1 на m2**/
		static vector<float> multiply(vector<float> m1, vector<float> m2){
			vector<float> res;
			res.push_back(m1[0]*m2[0]+m1[1]*m2[4]+m1[2]*m2[8]+m1[3]*m2[12]);
			res.push_back(m1[0]*m2[1]+m1[1]*m2[5]+m1[2]*m2[9]+m1[3]*m2[13]);
			res.push_back(m1[0]*m2[2]+m1[1]*m2[6]+m1[2]*m2[10]+m1[3]*m2[14]);
			res.push_back(m1[0]*m2[3]+m1[1]*m2[7]+m1[2]*m2[11]+m1[3]*m2[15]);
			res.push_back(m1[4]*m2[0]+m1[5]*m2[4]+m1[6]*m2[8]+m1[7]*m2[12]);
			res.push_back(m1[4]*m2[1]+m1[5]*m2[5]+m1[6]*m2[9]+m1[7]*m2[13]);
			res.push_back(m1[4]*m2[2]+m1[5]*m2[6]+m1[6]*m2[10]+m1[7]*m2[14]);
			res.push_back(m1[4]*m2[3]+m1[5]*m2[7]+m1[6]*m2[11]+m1[7]*m2[15]);
			res.push_back(m1[8]*m2[0]+m1[9]*m2[4]+m1[10]*m2[8]+m1[11]*m2[12]);
			res.push_back(m1[8]*m2[1]+m1[9]*m2[5]+m1[10]*m2[9]+m1[11]*m2[13]);
			res.push_back(m1[8]*m2[2]+m1[9]*m2[6]+m1[10]*m2[10]+m1[11]*m2[14]);
			res.push_back(m1[8]*m2[3]+m1[9]*m2[7]+m1[10]*m2[11]+m1[11]*m2[15]);
			res.push_back(m1[12]*m2[0]+m1[13]*m2[4]+m1[14]*m2[8]+m1[15]*m2[12]);
			res.push_back(m1[12]*m2[1]+m1[13]*m2[5]+m1[14]*m2[9]+m1[15]*m2[13]);
			res.push_back(m1[12]*m2[2]+m1[13]*m2[6]+m1[14]*m2[10]+m1[15]*m2[14]);
			res.push_back(m1[12]*m2[3]+m1[13]*m2[7]+m1[14]*m2[11]+m1[15]*m2[15]);
			return res;
		}

		/**стиль с уровнем**/
		class LevelRGB{
		public:
				int level;
				float rgb[3];
				LevelRGB(){}
				LevelRGB(int n, float ar[3]){
					this->level = n;
					this->rgb[0] = ar[0];
					this->rgb[1] = ar[1];
					this->rgb[2] = ar[2];
				}
				~LevelRGB(){ }
		};

		/*стили файла DWF*/
		class StyleDWF {
		private:
				/*стиль диффузный*/
				//std::map<std::int32_t , float[3]> _diffuse;
				std::vector<LevelRGB> _diffuse;
				/*стиль отражающий*/
				std::vector<LevelRGB> _specular;
				/*стиль рассеянный*/
				std::vector<LevelRGB> _mirror;
				/*стиль излучающий*/
				std::vector<LevelRGB> _emissive;
				/*стиль блеск*/
				std::vector<LevelRGB> _gloss;
				/*стиль прозрачность*/
				std::vector<LevelRGB> _transparence;

				bool deleteDiffuse(int level){
					int size = this->_diffuse.size();
					if (size > 0){
						//LevelRGB ob = this->_diffuse.at(size - 1);
						if (this->_diffuse.at(size - 1).level > level) {
							this->_diffuse.pop_back();
							return true;
						}
					}

					return false;
				}

				bool deleteSpecular(int level){
					int size = this->_specular.size();
					if (size > 0){
						if (this->_specular.at(size - 1).level > level) {
							this->_specular.pop_back();
							return true;
						}
					}

					return false;
				}

				bool deleteMirror(int level){
					int size = this->_mirror.size();
					if (size > 0){
						if (this->_mirror.at(size - 1).level > level) {
							this->_mirror.pop_back();
							return true;
						}
					}

					return false;
				}

				bool deleteEmissive(int level){
					int size = this->_emissive.size();
					if (size > 0){
						if (this->_emissive.at(size - 1).level > level) {
							this->_emissive.pop_back();
							return true;
						}
					}

					return false;
				}

				bool deleteGloss(int level){
					int size = this->_gloss.size();
					if (size > 0){
						if (this->_gloss.at(size - 1).level > level) {
							this->_gloss.pop_back();
							return true;
						}
					}

					return false;
				}

				bool deleteTransparence(int level){
					int size = this->_transparence.size();
					if (size > 0){
						if (this->_transparence.at(size - 1).level > level) {
							this->_transparence.pop_back();
							return true;
						}
					}

					return false;
				}
		public:
				StyleDWF(){}

				~StyleDWF(){}

				void addDiffuse(int level, float rgb[3]){
					int size = this->_diffuse.size();
					if (size > 0){
						LevelRGB ob = this->_diffuse.at(size - 1);
						if (ob.level >= level)
							return;
					}
					this->_diffuse.emplace_back(level, rgb);
				}

				void addSpecular(int level, float rgb[3]){
					int size = this->_specular.size();
					if (size > 0){
						LevelRGB ob = this->_specular.at(size - 1);
						if (ob.level >= level)
							return;
					}
					this->_specular.emplace_back(level, rgb);
				}

				void addMirror(int level, float rgb[3]){
					int size = this->_mirror.size();
					if (size > 0){
						LevelRGB ob = this->_mirror.at(size - 1);
						if (ob.level >= level)
							return;
					}
					this->_mirror.emplace_back(level, rgb);
				}

				void addEmissive(int level, float rgb[3]){
					int size = this->_emissive.size();
					if (size > 0){
						LevelRGB ob = this->_emissive.at(size - 1);
						if (ob.level >= level)
							return;
					}
					this->_emissive.emplace_back(level, rgb);
				}

				void addGloss(int level, float gloss){
					int size = this->_gloss.size();
					if (size > 0){
						LevelRGB ob = this->_gloss.at(size - 1);
						if (ob.level >= level)
							return;
					}
					float rgb[3]{gloss, gloss, gloss};
					this->_gloss.emplace_back(level, rgb);
				}

				void addTransparence(int level, float rgb[3]){
					int size = this->_transparence.size();
					if (size > 0){
						LevelRGB ob = this->_transparence.at(size - 1);
						if (ob.level >= level)
							return;
					}
					this->_transparence.emplace_back(level, rgb);
				}

				LevelRGB* getDiffuse(){
					if (this->_diffuse.size() > 0)
						return &this->_diffuse.at(this->_diffuse.size()-1);

					return NULL;
				}

				LevelRGB* getSpecular(){
					if (this->_specular.size() > 0)
						return &this->_specular.at(this->_specular.size()-1);

					return NULL;
				}

				LevelRGB* getMirror(){
					if (this->_mirror.size() > 0)
						return &this->_mirror.at(this->_mirror.size()-1);

					return NULL;
				}

				LevelRGB* getEmissive(){
					if (this->_emissive.size() > 0)
						return &this->_emissive.at(this->_emissive.size()-1);

					return NULL;
				}

				LevelRGB* getGloss(){
					if (this->_gloss.size() > 0)
						return &this->_gloss.at(this->_gloss.size()-1);

					return NULL;
				}

				LevelRGB* getTransparence(){
					if (this->_transparence.size() > 0)
						return &this->_transparence.at(this->_transparence.size()-1);

					return NULL;
				}

				void deleteLevel(int level){
					this->deleteDiffuse(level);
					this->deleteSpecular(level);
					this->deleteMirror(level);
					this->deleteEmissive(level);
					this->deleteGloss(level);
					this->deleteTransparence(level);
				}

				/**передаёт текущий стиль в SCALA**/
				void send(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler);
		};

		/*матрица с уровнем*/
		class LevelMatrix{
		public:
				int level;
				vector<float> matrix;
				LevelMatrix(){}
				LevelMatrix(int n, vector<float> m){
					this->level = n;
					this->matrix = m;
				}
		};

		/*матрицы файла DWF*/
		class MatrixDWF {
		private:
				/*матрицы с уровнем*/
				std::vector<LevelMatrix> _parts;
		public:
				MatrixDWF(){}

				~MatrixDWF(){}

				/**добавляет текущую матрицу**/
				bool add(int level, vector<float> m){
					int size = this->_parts.size();
					if (size == 0){
						this->_parts.emplace_back(level, m);
						return true;
					}
					if (_parts.at(size - 1).level >= level) return false;

					this->_parts.emplace_back(level, multiply(m, _parts.at(size - 1).matrix));
					return true;
				}

				/**возращает ссылку на матрицу с уровнем**/
				LevelMatrix* get(){
					if (this->_parts.size() > 0)
						return &this->_parts.at(this->_parts.size()-1);

					return NULL;
				}

				/**удаляет текущую матрицу**/
				bool remove(int level){
					int size = this->_parts.size();
					if (size > 0){
						if (this->_parts.at(size - 1).level > level) {
							this->_parts.pop_back();
							return true;
						}
					}

					return false;
				}

				/**передаёт текущую матрицу в SCALA**/
				void send(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler) {
					int	size = this->_parts.size();
					if (size > 0){
						javaHandler->handleMatrix(this->_parts.at(size - 1).matrix);
					}
				}

				/**возращает актуальную матрицу 4х4**/
				vector<float> getMatrix(){
					int	size = this->_parts.size();
					if (size > 0)
						return this->_parts.at(size - 1).matrix;

					vector<float> v = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
					return v;
				}
		};

		/**3D-сегмент**/
		class MeshDWF{
		public:
				/**координаты меша**/
				vector<float> xyz;
				/**индексы меша**/
				vector<int> indexes;

				MeshDWF() {}

				~MeshDWF(){}
		};

		/**базовый экземпляр сегмента для ссылок**/
		class RefSegDWF{
		private:
				/**матрица экземпляра**/
				vector<float> _m;
		public:
				/**название экземпляра(ключ)**/
				wstring name;
				/**части геометрии**/
				vector<MeshDWF> meshes;


				RefSegDWF (){}

				RefSegDWF (const std::wstring& name) : name(name) {}

				~RefSegDWF(){}

				/**устанавливает значение матрицы**/
				void SetMatrix(vector<float> m){
					if (_m.size() == 0)
						_m = m;
					else
						_m = multiply(m, _m);
				}

				/**возращает матрицу экземпляра**/
				vector<float> GetMatrix(){
					if (_m.size() > 0)
						return _m;

					vector<float> v = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
					return v;
				}
		};

		/**слой для вызова метода doImportNextLayer**/
		class LayerDWF{
		private:
				/**секции файла DWF**/
				std::vector<int> _levels;
		public:
				/**семантика дочерних узлов слоя**/
				std::map<std::wstring, OBJ_SEMANTIC> semantics;
				/**название слоя**/
				wstring name;
				/**название колонок**/
				std::map<std::wstring, std::wstring> fields;
				/*Глубина в структуре узлов, на которой находится геометрия нового объекта*/
				int32_t geomLevel = -1;

				LayerDWF (){}

				LayerDWF (const std::wstring& name) : name(name) {}

				~LayerDWF(){
					name = L"";
				}

				/**чтение семантики слоя**/
				bool ReadSemantic(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst);

				/**чистит текущие значения семантики**/
				void clearValue(){
					for (auto &keyVal: this->fields) {
						keyVal.second = L"";
					}
				}

				/**устанавливает текущие значения семантики**/
				void setValues(OBJ_SEMANTIC sem){
					this->clearValue();
					for (auto const &semv : sem){
						this->fields[semv.propName] = semv.propValue;
					}
				}

				/**возращает список значений семантики**/
				vector<wstring> getValues(){
					vector<wstring> result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back(pos->second);
					}

					return result;
				}

				/**возращает название колонок**/
				vector<wstring> getFields(){
					std::vector<wstring> result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back(pos->first);
					}

					return result;
				}

				/**возращает семантику узла дерева**/
				OBJ_SEMANTIC getOBJ_SEMANTIC(){
					OBJ_SEMANTIC result;
					for (auto pos = this->fields.begin(); pos != this->fields.end(); ++pos) {
						result.push_back((dgn::SemanticValue(pos->first, pos->second)));
					}

					return result;
				}

				/**Добавляет узел семантики**/
				void openNode(int level){
					_levels.push_back(level);
				}

				/**Возращает true, если на данном уровне имелась семантика**/
				bool closeNode(int level){
					int size = this->_levels.size();
					if (size > 0){
						if (this->_levels.at(size - 1) > level) {
							this->_levels.pop_back();
							return true;
						}
					}

					return false;
				}
		};

		/**секция файла DWF**/
		class SectionDWF{
		public:
				/**Глубина в структуре узлов, на которой находятся слои**/
				int32_t levelLayers = -1;
				/**слои секции**/
				std::map<std::wstring, LayerDWF> layers;
				/**Текущий обрабатываемый слой**/
				LayerDWF *layer = NULL;
				/**Текущий стиль**/
				StyleDWF styleDWF;
				/**матрицы файла DWF**/
				MatrixDWF matrixDWF;
				/**экземпляры возможных примитивов геометрии**/
				std::map<std::wstring, RefSegDWF> references;
				/**Текущий обрабатываемый базовый экземпляр**/
				RefSegDWF *reference = NULL;

				SectionDWF(){ };

				~SectionDWF(){ }

				/**предворительное чтение структуры и семантики**/
				bool SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler);
		};


		/**реализация синглтона утилит для вызова из Java**/
		class DwfUtilsImpl : public dgn::DwfUtils {
		private:
				/**предворительное чтение структуры(порядок и название слоёв)**/
				void readTreeStructureFromGraphics(const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler, DWFSection *pSection);
		protected:
				/**Содержит информацию о файле DWF**/
				DWFToolkit::DWFPackageReader::tPackageInfo tInfo;

		public:
				DWFToolkit::DWFPackageReader oReader;
				/**Из какого файла читать**/
				const std::wstring dwfFile;
				/**Глубина в структуре узлов, на которой находятся слои**/
				const int32_t layerDepth;
				/**секции файла DWF**/
				std::vector<SectionDWF> sections;
				/**Текущая секция**/
				SectionDWF *section;

				DwfUtilsImpl(const std::wstring & dwfFile, int32_t layerDepth);

				~DwfUtilsImpl() override { }


				/**предворительное чтение структуры**/
				void readTreeStructure(const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler) override;
				/**загрузка файла в базу**/
				void readAllLayers(const std::shared_ptr<dgn::DwfLayersFillStreamHandler> &handler) override;

				/**конвертор строки**/
				static DWFString toDwfString(std::wstring str){
					DWFString name;
					name.append(str.c_str());
					return name;
				}
				/**конвертор строки**/
				static std::wstring toStdString(const DWFString &str){
					return std::wstring((const wchar_t *)str, str.chars());
				}
				/**получение имени слоя из значений семантики**/
				static std::wstring getName(OBJ_SEMANTIC sem){
					for (auto const &s : sem){
						if (s.propName.compare(L"_name") == 0)
							return s.propValue;
					}

					return L"";
				}
				/**разделение семантики на названия колонок**/
				static std::pair<vector<wstring>, vector<wstring>> getPair(OBJ_SEMANTIC sem){
					vector<wstring> fields;
					vector<wstring> values;
					for (auto const &s : sem){
						fields.push_back(s.propName);
						values.push_back(s.propValue);
					}

					return {fields, values};
				}
		};


}
