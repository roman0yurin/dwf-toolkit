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

		class LevelRGB{
		public:
				int level;
				float rgb[3];
				LevelRGB(){};
				LevelRGB(int n, float ar[3]){
					this->level = n;
					this->rgb[0] = ar[0];
					this->rgb[1] = ar[1];
					this->rgb[2] = ar[2];
				};
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
				StyleDWF(){};

				void addDiffuse(int level, float rgb[3]){
					int size = this->_diffuse.size();
					if (size > 0){
						LevelRGB ob = this->_diffuse.at(size - 1);
						if (ob.level >= level)
							return;
					}
					LevelRGB* ob = new LevelRGB(level, rgb);
					this->_diffuse.push_back(*ob);
				}

				void addSpecular(int level, float rgb[3]){
					int size = this->_specular.size();
					if (size > 0){
						LevelRGB ob = this->_specular.at(size - 1);
						if (ob.level >= level)
							return;
					}
					LevelRGB* ob = new LevelRGB(level, rgb);
					this->_specular.push_back(*ob);
				}

				void addMirror(int level, float rgb[3]){
					int size = this->_mirror.size();
					if (size > 0){
						LevelRGB ob = this->_mirror.at(size - 1);
						if (ob.level >= level)
							return;
					}
					LevelRGB* ob = new LevelRGB(level, rgb);
					this->_mirror.push_back(*ob);
				}

				void addEmissive(int level, float rgb[3]){
					int size = this->_emissive.size();
					if (size > 0){
						LevelRGB ob = this->_emissive.at(size - 1);
						if (ob.level >= level)
							return;
					}
					LevelRGB* ob = new LevelRGB(level, rgb);
					this->_emissive.push_back(*ob);
				}

				void addGloss(int level, float gloss){
					int size = this->_gloss.size();
					if (size > 0){
						LevelRGB ob = this->_gloss.at(size - 1);
						if (ob.level >= level)
							return;
					}
					LevelRGB* ob = new LevelRGB();
					ob->level = level;
					ob->rgb[0] = gloss;
					ob->rgb[1] = gloss;
					ob->rgb[2] = gloss;
					this->_gloss.push_back(*ob);
				}

				void addTransparence(int level, float rgb[3]){
					int size = this->_transparence.size();
					if (size > 0){
						LevelRGB ob = this->_transparence.at(size - 1);
						if (ob.level >= level)
							return;
					}
					LevelRGB* ob = new LevelRGB(level, rgb);
					this->_transparence.push_back(*ob);
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

				/*
				void addDiffuse(int32_t level, float red, float green, float blue){
					int r = (int)red*256;
					int g = 0 << (int)green*256;
					int b = 0 << 0 << (int)blue*256;
					int result = r+g+b;
					this->_diffuse[level] = result;
				}
				int getDiffuse(){
					if (this->_diffuse.size() > 0)
						return this->_diffuse.end().operator*().second;
					return -1;
				}
				 */
				void deleteLevel(int level){
					this->deleteDiffuse(level);
					this->deleteSpecular(level);
					this->deleteMirror(level);
					this->deleteEmissive(level);
					this->deleteGloss(level);
					this->deleteTransparence(level);
					/*
					bool log = true;
					while (log){
						log = this->deleteDiffuse(level);
					}
					log = true;
					while (log){
						log = this->deleteSpecular(level);
					}
					log = true;
					while (log){
						log = this->deleteMirror(level);
					}
					log = true;
					while (log){
						log = this->deleteEmissive(level);
					}
					log = true;
					while (log){
						log = this->deleteGloss(level);
					}
					log = true;
					while (log){
						log = this->deleteTransparence(level);
					}
					 /*
					/*
					int size = this->_diffuse.size();
					if (size > 0){
						LevelRGB ob = this->_diffuse.at(size - 1);
						if (ob.level == level)
							this->_diffuse.pop_back();
					}
					 */
				}

				void send(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler) {
					dgn::DwfColor* dwfColor;
					int	size = this->_diffuse.size();
					if (size > 0){
						LevelRGB ob = this->_diffuse.at(size - 1);
						dwfColor = new dgn::DwfColor(ob.rgb[0], ob.rgb[1], ob.rgb[2], 1);
						javaHandler->handleColor(dgn::ColorType::DIFFUSE, *dwfColor);
					}

					size = this->_specular.size();
					if (size > 0){
						LevelRGB ob = this->_specular.at(size - 1);
						dwfColor = new dgn::DwfColor(ob.rgb[0], ob.rgb[1], ob.rgb[2], 1);
						javaHandler->handleColor(dgn::ColorType::SPECULAR, *dwfColor);
					}

					size = this->_mirror.size();
					if (size > 0){
						LevelRGB ob = this->_mirror.at(size - 1);
						dwfColor = new dgn::DwfColor(ob.rgb[0], ob.rgb[1], ob.rgb[2], 1);
						javaHandler->handleColor(dgn::ColorType::AMBIENT, *dwfColor);
					}

					size = this->_emissive.size();
					if (size > 0){
						LevelRGB ob = this->_emissive.at(size - 1);
						dwfColor = new dgn::DwfColor(ob.rgb[0], ob.rgb[1], ob.rgb[2], 1);
						javaHandler->handleColor(dgn::ColorType::EMISSIVE, *dwfColor);
					}

					size = this->_gloss.size();
					if (size > 0){
						LevelRGB ob = this->_gloss.at(size - 1);
						dwfColor = new dgn::DwfColor(ob.rgb[0], ob.rgb[1], ob.rgb[2], 1);
						javaHandler->handleColor(dgn::ColorType::SHININESS, *dwfColor);
					}

					size = this->_transparence.size();
					if (size > 0){
						LevelRGB ob = this->_transparence.at(size - 1);
						dwfColor = new dgn::DwfColor(ob.rgb[0], ob.rgb[1], ob.rgb[2], 1);
						javaHandler->handleColor(dgn::ColorType::OPACITY, *dwfColor);
					}
				}
		};


		class LevelMatrix{
		public:
				int level;
				vector<float> matrix;
				LevelMatrix(){};
				LevelMatrix(int n, vector<float> m){
					this->level = n;
					this->matrix = m;
				};
		};

		/*матрицы файла DWF*/
		class MatrixDWF {
		private:
				/*матрицы с уровнем*/
				std::vector<LevelMatrix> _parts;

				/**Перемножить матрицы 4x4 m1 на m2**/
				vector<float> multiply(vector<float> m1, vector<float> m2){
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

		public:
				MatrixDWF(){};

				bool add(int level, vector<float> m){
					int size = this->_parts.size();
					if (size == 0){
						LevelMatrix* ob = new LevelMatrix(level, m);
						this->_parts.push_back(*ob);
						return true;
					}
					if (_parts.at(size - 1).level >= level) return false;

					LevelMatrix* ob = new LevelMatrix(level, this->multiply(_parts.at(size - 1).matrix, m));
					this->_parts.push_back(*ob);
					return true;
				}

				LevelMatrix* get(){
					if (this->_parts.size() > 0)
						return &this->_parts.at(this->_parts.size()-1);

					return NULL;
				}

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

				void send(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler) {
					int	size = this->_parts.size();
					if (size > 0){
						javaHandler->handleMatrix(this->_parts.at(size - 1).matrix);
					}
				}
		};

		class SectionDWF;
		class CurrentSemantic{
		public:
				int level;
				OBJ_SEMANTIC semantic;
				CurrentSemantic(){};
				CurrentSemantic(int level, OBJ_SEMANTIC semantic){
					this->level = level;
					this->semantic = semantic;
				};
		};

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
		//		CurrentSemantic *_semantic = NULL;
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
				/*Семантика текущего узла дерева*/
				std::vector<CurrentSemantic> semantic;

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

				void deleteSemantic(int level){
					int size = this->semantic.size();
					if (size > 0){
						//CurrentSemantic ob = this->semantic.at(size - 1);
						if (this->semantic.at(size - 1).level > level)
							this->semantic.pop_back();
					}
				}

				void setValues(OBJ_SEMANTIC sem){
					for (auto const &semv : sem){
						if (this->fields[semv.propName] == L"")
							this->fields[semv.propName] = semv.propValue;
						//else
						//	this->fields[semv.propName] += L"\\" + semv.propValue;
					}
				}

				void setValues(){
					this->clearValue();
					for (auto pos = this->semantic.begin(); pos != this->semantic.end(); ++pos) {
						OBJ_SEMANTIC sem = pos.base()->semantic;
						this->setValues(sem);
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
				/**Текущий стиль**/
				StyleDWF *styleDWF;
				/**Текущая матрица**/
				MatrixDWF *matrixDWF;

				SectionDWF(){
					this->styleDWF = new StyleDWF();
					this->matrixDWF = new MatrixDWF();
				};
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