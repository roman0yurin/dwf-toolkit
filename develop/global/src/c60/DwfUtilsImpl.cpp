//
// Created by den on 05.07.18.
//

#include <dgn/ColorType.hpp>
#include "DwfUtilsImpl.h"
#include "JLogger.h"

//typedef std::vector<dgn::SemanticValue> OBJ_SEMANTIC;
//using namespace c60;


c60::DwfUtilsImpl::DwfUtilsImpl(const std::wstring & dwfFile, int32_t layerDepth):
				dgn::DwfUtils(),
				layerDepth(layerDepth),
				dwfFile(dwfFile),
				oReader(DWFFile(toDwfString(dwfFile)))
{
	oReader.getPackageInfo(this->tInfo);
}

/*
c60::DwfUtilsImpl::~DwfUtilsImpl() {
	this->layer = NULL;
	this->pDef = NULL;
	this->pW3DStream = NULL;
	DWFCORE_FREE_OBJECT(this->layers);
}
 */

/**получить синглтон утилит */
std::shared_ptr<dgn::DwfUtils> dgn::DwfUtils::getInstance(const std::wstring & dwfFile, int32_t layerDepth){
	return std::static_pointer_cast<dgn::DwfUtils>(std::shared_ptr<c60::DwfUtilsImpl>(new c60::DwfUtilsImpl(dwfFile, layerDepth)));
}





class OpenHandler : public TK_Open_Segment {
protected:
		//std::map<std::wstring, OBJ_SEMANTIC> &semantic;
		c60::SectionDWF *section;
		std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler;
public:
		//OpenHandler(std::shared_ptr<dgn::DwfStreamHandler> javaHandler,  std::map<std::wstring, OBJ_SEMANTIC> &semantic) : TK_Open_Segment(), semantic(semantic) {
		OpenHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler,  c60::SectionDWF *section) : TK_Open_Segment() {
			this->javaHandler = javaHandler;
			this->section = section;
		}

		virtual ~OpenHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {

			TK_Status eStatus = TK_Open_Segment::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				int level = rW3DParser.getNestingLevel();
				if (level >= this->section->levelLayers) {
					if (level == this->section->levelLayers) {
						/**читаем новый слой**/
						string str = string(this->m_string, this->m_length);
						wstring wstr(str.begin(), str.end());
						this->section->layer = &this->section->layers[wstr];
						this->javaHandler->openLayer();
					} else {
						//if (level <= this->section->layer->geomLevel) {
							string str = string(this->m_string, this->m_length);
							wstring wstr(str.begin(), str.end());
							if (!wstr.empty()) {
								/*читаем семантику текущего узла*/
								c60::OBJ_SEMANTIC semantic = this->section->layer->semantics[wstr];
								//this->section->layer->setCurrentSemantic(semantic);
								//this->section->layer->bottomLevel = level;
								this->section->layer->setValues(semantic);
							}
						//}
					}
				}
			}
//
//			string str =  string(this->m_string, this->m_length);
//			wstring wstr(str.begin(), str.end());
			//if ()
//			auto const result = semantic.find(wstr);
//			if (result != semantic.end()){
//				OBJ_SEMANTIC sem = result->second;
//				for(auto const &pair : sem){
//					wcout << pair.first << L" -> " << pair.second << endl;
//				}
//			}
//			wcout << L"\n" << endl;
			///javaHandler->openNode(semantic[wstr]);
			return eStatus;
		}
};

class CloseHandler: public TK_Close_Segment{
protected:
		c60::SectionDWF *section;
		std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler;
public:
		CloseHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler,  c60::SectionDWF *section) : TK_Close_Segment(){
			this->javaHandler = javaHandler;
			this->section = section;
		}

		TK_Status Execute(BStreamFileToolkit &rW3DParser){
			TK_Status eStatus = TK_Close_Segment::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				int level = rW3DParser.getNestingLevel();
				if (level > this->section->levelLayers - 2) {
					if (level == this->section->levelLayers - 1) {
						/*прочитали слой*/
						JLogger::info(L"C++ close layer %S", this->section->layer->name.c_str());
						javaHandler->closeLayer();
						this->section->layer = NULL;
					} else {
						/*
						if (level < this->section->layer->bottomLevel){
							this->section->layer->setTopSemantic();
						}
						if (level == this->section->layer->geomLevel - 1){
							this->section->layer->geomLevel = this->section->levelLayers;
							javaHandler->closeNode();
						}
						 */
						if (level == this->section->levelLayers) {
							//прочитали дерево объектов
							this->section->layer->clearValue();
						//} else{
						//	javaHandler->openChildNode();
						}
						if (level == this->section->layer->geomLevel - 1){
							this->section->layer->geomLevel = this->section->levelLayers;
							javaHandler->closeNode();
						}
					}
				}
			}
			return eStatus;
		}
};

class ShellHandler : public TK_Shell {
protected:
		std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler;
public:
		ShellHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler) : TK_Shell() {
			this->javaHandler = javaHandler;
		}

		virtual ~ShellHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &parser) {
			TK_Status status = TK_Shell::Execute(parser);
			if (status == TK_Normal) {
				int level = parser.getNestingLevel();
				dgn::Mesh mesh = this->toDgnMesh();
				JLogger::info(L"#################num= %u", mesh.xyz.size());
				//unsigned long num = mesh.xyz.size()/3;
				//JLogger::info(L"count= %u", num);
				int j = 0;
				for (int i = 0; i < 9; ++i){
					float x = mesh.xyz[i];
					if (j == 0)
						JLogger::info(L"x= %f", x);
					else{
						if (j == 1)
							JLogger::info(L"y= %f", x);
						else
							JLogger::info(L"z= %f", x);
					}
					if (j == 2)
						j = 0;
					else
						++j;
				}
				this->javaHandler->handleMesh(mesh);
			}
			return status;
		}
};

class ColorHandler : public TK_Color {
protected:
		c60::SectionDWF *section;
		std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler;
public:
		ColorHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler,  c60::SectionDWF *section) : TK_Color() {
		//ColorHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler) : TK_Color() {
			this->javaHandler = javaHandler;
			this->section = section;
		}

		virtual ~ColorHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
			TK_Status eStatus = TK_Color::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				dgn::DwfColor* dwfColor;
				int level = rW3DParser.getNestingLevel();
				if (level > this->section->levelLayers) {
					if ((this->m_channels & (1 << TKO_Channel_Diffuse)) != 0) {
						dwfColor = new dgn::DwfColor(this->m_diffuse.m_rgb[0], this->m_diffuse.m_rgb[1], this->m_diffuse.m_rgb[2], 0);
						if (level == this->section->layer->geomLevel) {
							this->javaHandler->handleColor(dgn::ColorType::DIFFUSE, *dwfColor);
						} else{
							this->javaHandler->handleColor(dgn::ColorType::AMBIENT, *dwfColor);
						}
					}
					if ((m_channels & (1 << TKO_Channel_Specular)) != 0) {
						dwfColor = new dgn::DwfColor(this->m_specular.m_rgb[0], this->m_specular.m_rgb[1], this->m_specular.m_rgb[2], 0);
						this->javaHandler->handleColor(dgn::ColorType::SPECULAR, *dwfColor);
					}
					if ((m_channels & (1 << TKO_Channel_Transmission)) != 0) {
						dwfColor = new dgn::DwfColor(this->m_transmission.m_rgb[0], this->m_transmission.m_rgb[1], this->m_transmission.m_rgb[2], 0);
						this->javaHandler->handleColor(dgn::ColorType::OPACITY, *dwfColor);
					}
				} else{
					dwfColor = new dgn::DwfColor(this->m_diffuse.m_rgb[0], this->m_diffuse.m_rgb[1], this->m_diffuse.m_rgb[2], 0);
				}
			}
			return eStatus;
		}
};

class MatrixHandler : public TK_Matrix {
protected:
		c60::SectionDWF *section;
		std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler;
public:
		MatrixHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler, c60::SectionDWF *section) : TK_Matrix(0) {
			this->javaHandler = javaHandler;
			this->section = section;
		}

		virtual ~MatrixHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
			TK_Status status = TK_Matrix::Execute(rW3DParser);
			if (status == TK_Normal) {
				vector<float> matrix;
				for (int i = 0; i < 16; ++i) {
					matrix.push_back(this->m_matrix[i]);
				}
				int level = rW3DParser.getNestingLevel();
				if (level > this->section->levelLayers) {
					this->section->layer->geomLevel = level;
					vector<wstring> values = this->section->layer->getValues();
					this->javaHandler->openNode(values);
					this->javaHandler->handleMatrix(matrix);
				} else{
					this->javaHandler->handleSectionMatrix(matrix);
				}
			}
			return status;
		}
};

/*
wstring getMin(std::map<std::wstring, float > items){
	auto result = items.begin();
	for (auto pos = items.begin(); pos != items.end(); ++pos){
		if (pos->second < result->second)
			result = pos;
	}
	if (result == items.end())
		return L"";

	return result->first;
}
 */


//bool c60::LayerDWF::SetFields() {
	/*
	std::map<std::wstring, float > items;
	for (auto i = this->semantics.begin(); i != this->semantics.end(); ++i) {
		OBJ_SEMANTIC semantic = this->semantics.begin()->second;
		int t = 0;
		for (auto pos = i->second.begin(); pos != i->second.end(); ++pos) {
			auto ob = items.find(pos->propName);
			if (ob == items.end()){
				items[pos->propName] = t;
			} else{
				ob->second = (ob->second + t)*0.5;
			}
			++t;
		}
	}
	while (items.size() > 0) {
		wstring item = getMin(items);
		this->fields[item] = L"";
		//this->fields.push_back(item);
		items.erase(item);
	}
	 */
	/*
	for (auto i = this->semantics.begin(); i != this->semantics.end(); ++i) {
		for (auto pos = i->second.begin(); pos != i->second.end(); ++pos) {
			auto ob = this->fields.find(pos->propName);
			if (ob == this->fields.end()) {
				this->fields[pos->propName] = L"";
			}
		}
	}
	if (this->fields.size() == 0)
		return false;
	return true;
}
*/

	/*
bool c60::LayerDWF::GetSemantic(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst){
	int32_t currentlevel = level + 1;
	OBJ_SEMANTIC semantic;
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			//wstring name = DwfUtilsImpl::toStdString(pProp->name());
			//wstring value = DwfUtilsImpl::toStdString(pProp->value());

			//auto ob = this->fields.find(name);
			//if (ob == this->fields.end())
			//	this->fields[name] = value;
			//else
			//	ob->second = value;

			//semantic.push_back(dgn::SemanticValue(name, value));
			this->fields[DwfUtilsImpl::toStdString(pProp->name())] = DwfUtilsImpl::toStdString(pProp->value());
		}

		DWFCORE_FREE_OBJECT(piProp);
	}

	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			if (!this->GetSemantic(currentlevel, pDef, piChildren->value()))
				return false;
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}

	return true;
}
*/

bool c60::LayerDWF::ReadSemantic(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst) {
	int32_t currentlevel = level + 1;
	OBJ_SEMANTIC semantic;
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			wstring name = DwfUtilsImpl::toStdString(pProp->name());
			wstring value = DwfUtilsImpl::toStdString(pProp->value());
			semantic.push_back(dgn::SemanticValue(name, value));
			this->fields[name] = value;
		}

		DWFCORE_FREE_OBJECT(piProp);
	}
	this->semantics[DwfUtilsImpl::toStdString(pInst->object())] = semantic;

	DWFString str = pInst->id();
	std::wstring id = DwfUtilsImpl::toStdString(str);
	JLogger::info(L"ID= %S", id.c_str());
	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			if (!this->ReadSemantic(currentlevel, pDef, piChildren->value()))
				return false;
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}

	//this->semantics[DwfUtilsImpl::toStdString(pInst->object())] = this->getOBJ_SEMANTIC();
	return true;
}

/*
void c60::LayerDWF::SetFields(DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst) {
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			this->fields.push_back(toStdString(pProp->name()));
		}

		DWFCORE_FREE_OBJECT(piProp);
	}
}
 */
/*
void c60::LayerDWF::SetBottomLevel(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst) {
	int32_t currentlevel = level + 1;
	if (this->bottomLevel < currentlevel)
		this->bottomLevel = currentlevel;

	OBJ_SEMANTIC semantic;
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			semantic.push_back(dgn::SemanticValue(toStdString(pProp->name()), toStdString(pProp->value())));
		}

		DWFCORE_FREE_OBJECT(piProp);
	}

	this->semantics[toStdString(pInst->object())] = semantic;
	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			this->SetBottomLevel(currentlevel, pDef, piChildren->value());
			if (this->semanticLevel == currentlevel){
				if (this->fields.size() == 0){
					this->SetFields(pDef, piChildren->value());
				}
			}
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}
}
 */

//, std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler
//**считаем кол-во объектов**//
//void SetCountObject(int32_t level, int32_t layerLevel, int32_t &countLayer, int32_t &countOb, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst){
//void SetLayers(int32_t level, int32_t layerLevel, int32_t &countLayer, int32_t &countOb, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst){
/*
bool c60::SectionDWF::SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler){
	int32_t currentlevel = level + 1;
	if (currentlevel < this->levelLayers) {
		DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
		if (piChildren) {
			for (; piChildren->valid(); piChildren->next()) {
				if (!this->SetLayers(currentlevel, pDef, piChildren->value(), handler)){
					DWFCORE_FREE_OBJECT(piChildren);
					return false;
				}
			}
			DWFCORE_FREE_OBJECT(piChildren);
			return true;
		}

		return false;
	}

	this->layer = NULL;
	if (currentlevel == this->levelLayers) {
		DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
		DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
		if (piProp) {
			if (piProp->valid()) {
				DWFProperty *pProp = piProp->value();
				wstring name_id = toStdString(pProp->name());
				if (name_id.compare(L"_name") == 0) {
					this->layer = new LayerDWF();
					this->layer->name = toStdString(pProp->value());
				}
			}

			DWFCORE_FREE_OBJECT(piProp);
		}

		if (this->layer != NULL) {
			DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
			if (piChildren) {
				for (; piChildren->valid(); piChildren->next()) {
					this->layer->SetBottomLevel(currentlevel, pDef, piChildren->value());
				}

				DWFCORE_FREE_OBJECT(piChildren);
			}

			handler->openLayer(this->layer->name);
			wstring key = toStdString(pInst->object());
			this->layers[key] = *this->layer;
			//this->layers[toStdString(pInst->object())] = *this->layer;
			LayerDWF layerDWF = this->layers[key];
			handler->closeLayer(this->layer->semantics.size(), this->layer->fields);
		}
	}

	return false;
}
 */

//void GetDWFObjectDefinition(int32_t level, int32_t &bottomLevel, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> &map_semantics){
//void GetDWFObjectDefinition(int32_t level, int32_t &bottomLevel, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> &map_semantics){
//void SetbottomLevel(int32_t level, int32_t &bottomLevel, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> semantics){
/*
bool c60::SectionDWF::SetLevelLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler) {
	int32_t currentlevel = level + 1;
	//if (currentlevel > this->bottomLevel)
	//	this->bottomLevel = currentlevel;

	//DWFProperty *pProp = NULL;
	OBJ_SEMANTIC semantic;
	wstring name = L"";
	wstring value = L"";
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			name = DwfUtilsImpl::toStdString(pProp->name());
			value = DwfUtilsImpl::toStdString(pProp->value());
			semantic.push_back(dgn::SemanticValue(name, value));
			//semantic.push_back(dgn::SemanticValue(toStdString(pProp->name()), toStdString(pProp->value())));
		}
		if (name.compare(L"_name") == 0){
			if (value.compare(L"Geometry") == 0){
				if (this->levelLayers == 0) {
					this->levelLayers = currentlevel - 2;
				}
				if (this->layer == NULL){
					this->layer = new LayerDWF();
					this->layer->semanticLevel = currentlevel - 1;
					this->layer->bottomLevel = currentlevel;
				}
			} else{
				if (value.compare(L"Volume") == 0) {
					if (this->levelLayers == 0) {
						this->levelLayers = currentlevel - 4;
					}
					if (this->layer == NULL) {
						this->layer = new LayerDWF();
						this->layer->semanticLevel = currentlevel - 2;
						this->layer->bottomLevel = currentlevel;
					}
				}
			}
		}

		DWFCORE_FREE_OBJECT(piProp);
	}
	//this->semantics[toStdString(pInst->object())] = semantic;

	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	//int32_t result = level + 1;
	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			//GetDWFObjectDefinition(currentlevel, bottomLevel, pDef, piChildren->value(), map_semantics);
			if (!SetLevelLayers(currentlevel, pDef, piChildren->value(), handler))
				return false;
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}

	if (this->layer != NULL) {
		if (currentlevel == this->layer->semanticLevel)
			this->layer->semantics[DwfUtilsImpl::toStdString(pInst->object())] = semantic;
		else{
			if (currentlevel == this->levelLayers){
				this->layer->name = value;
				if (!this->layer->SetFields()) return false;

				handler->openLayer(this->layer->name);
				wstring key = DwfUtilsImpl::toStdString(pInst->object());
				this->layers[key] = *this->layer;
				//this->layers[toStdString(pInst->object())] = *this->layer;
				//LayerDWF layerDWF = this->layers[key];
				std::vector<wstring> fields;
				for (auto pos = this->layer->fields.begin(); pos != this->layer->fields.end(); ++pos) {
					fields.push_back(pos->first);
				}
				handler->closeLayer(this->layer->semantics.size(), fields);
				this->layer = NULL;
			}
		}
	}

	return true;
}
 */

bool c60::SectionDWF::SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler) {
	int32_t currentlevel = level + 1;
	if (currentlevel < this->levelLayers){
		DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
		if (piChildren) {
			for (; piChildren->valid(); piChildren->next()) {
				if (!this->SetLayers(currentlevel, pDef, piChildren->value(), handler)) {
					DWFCORE_FREE_OBJECT(piChildren);
					return false;
				}
			}

			DWFCORE_FREE_OBJECT(piChildren);
			return true;
		}

		return false;
	}


	if (currentlevel == this->levelLayers) {
		wstring layerName = L"";
		DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
		DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
		if (piProp) {
			for (; piProp->valid(); piProp->next()) {
				DWFProperty *pProp = piProp->value();
				wstring name = DwfUtilsImpl::toStdString(pProp->name());
				if (name.compare(L"_name") == 0)
					layerName = DwfUtilsImpl::toStdString(pProp->value());
			}

			DWFCORE_FREE_OBJECT(piProp);
		}
		if (layerName.empty())
			return false;

		this->layer = new LayerDWF();
		this->layer->name = layerName;
		this->layer->geomLevel = this->levelLayers;
		handler->openLayer(this->layer->name);
		DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
		if (piChildren) {
			for (; piChildren->valid(); piChildren->next()) {
				if (!this->layer->ReadSemantic(currentlevel, pDef, piChildren->value()))
					return false;
			}
			DWFCORE_FREE_OBJECT(piChildren);
		}

		this->layer->clearValue();
		wstring key = DwfUtilsImpl::toStdString(pInst->object());
		this->layers[key] = *this->layer;
		handler->closeLayer(this->layer->semantics.size(), this->layer->getFields());
		this->layer = NULL;

		return true;
	}

	return false;
}

void c60::DwfUtilsImpl::readTreeStructure(const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler) {
	//ERROR(L"TEST ERROR %d", 123);
	if ((this->tInfo.eType != DWFPackageReader::eDWFPackage) && (this->tInfo.eType != DWFPackageReader::eDWFXPackage)) {
		return;
	}

	DWFManifest &rManifest = this->oReader.getManifest();
	//this->countLayer = 0;
	//this->countNode = 0;
	//std::map<std::wstring, OBJ_SEMANTIC> all_semantic;
	DWFSection *pSection = NULL;
	DWFManifest::SectionIterator *piSections = rManifest.getSections();
	if (piSections) {
		for (; piSections->valid(); piSections->next()) {
			pSection = piSections->get();
			pSection->readDescriptor();

			this->section = new SectionDWF();
			this->section->levelLayers = this->layerDepth;
			DWFDefinedObjectInstance::tList rRootInstances;
			DWFResourceContainer::ResourceIterator *piObjDefs = pSection->findResourcesByRole(DWFXML::kzRole_ObjectDefinition);
			if (piObjDefs && piObjDefs->valid()) {
				//JLogger::info(L"getObjectDefinition");
				DWFObjectDefinition *pDef = pSection->getObjectDefinition();
				//this->section->pDef = pSection->getObjectDefinition();
				if (pDef) {
					pDef->getRootInstances(rRootInstances);

					int32_t level = 0;
					//this->section->levelLayers = 0;
					//int32_t bottomLevel = 0;
					DWFDefinedObjectInstance *pInst = NULL;
					DWFDefinedObjectInstance::tList::const_iterator iInst = rRootInstances.begin();
					for (; iInst != rRootInstances.end(); iInst++) {
						pInst = *iInst;
						//GetDWFObjectDefinition(level, this->bottomLevel, pDef, pInst, this->semantics);
						this->section->SetLayers(level, pDef, pInst, handler);
						//this->section->SetLayers(level, pDef, pInst, handler);
						this->sections.push_back(*this->section);
					}
				}
			}
		}

		DWFCORE_FREE_OBJECT(piSections);
	}
}


void c60::DwfUtilsImpl::readAllLayers(const std::shared_ptr<dgn::DwfLayersFillStreamHandler> &handler) {

	DWFManifest &rManifest = this->oReader.getManifest();
	DWFSection *pSection = NULL;
	DWFManifest::SectionIterator *piSections = rManifest.getSections();
	if (piSections) {
		auto pos = this->sections.begin();
		for (; piSections->valid(); piSections->next()) {
			pSection = piSections->get();
			pSection->readDescriptor();
			this->section = pos.base();

			DWFResourceContainer::ResourceIterator *piGraphics = pSection->findResourcesByRole(DWFXML::kzRole_Graphics3d);
			if (piGraphics) {
				for (; piGraphics->valid(); piGraphics->next()) {
					DWFGraphicResource *pW3D = dynamic_cast<DWFGraphicResource *>(piGraphics->get());
					if (pW3D) {
						//
						// get the data stream
						//
						DWFCore::DWFInputStream *pW3DStream = pW3D->getInputStream();

						//
						// Create the HSF toolkit object that does the stream I/O
						//
						BStreamFileToolkit oW3DStreamParser;

						oW3DStreamParser.SetOpcodeHandler(TKE_Open_Segment, new OpenHandler(handler, this->section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Close_Segment, new CloseHandler(handler, this->section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Shell, new ShellHandler(handler));
						oW3DStreamParser.SetOpcodeHandler(TKE_Color, new ColorHandler(handler, this->section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Modelling_Matrix, new MatrixHandler(handler, this->section));

						//
						// Attach the stream to the parser
						//
						oW3DStreamParser.OpenStream(*pW3DStream);

						size_t nBytesRead = 0;
						char aBuffer[16384] = {0};

						//
						// read and process the stream
						//
						while (pW3DStream->available() > 0) {
							//
							// read from the stream ourselves, we could also use ReadBuffer()
							// but it basically just performs this same action.
							//
							nBytesRead = pW3DStream->read(aBuffer, 16384);

							//
							// use the parser to process the buffer
							//
							if (oW3DStreamParser.ParseBuffer(aBuffer, nBytesRead, TK_Normal) == TK_Error) {
								wcout << L"Error occured parsing buffer" << endl;
								break;
							}
						}

						//
						// Done with the stream, we must delete it
						//
						oW3DStreamParser.CloseStream();
						DWFCORE_FREE_OBJECT(pW3DStream);
					}
				}

				if (pos != this->sections.end())
					++pos;
			}
		}

		DWFCORE_FREE_OBJECT(piSections);
	}
}
