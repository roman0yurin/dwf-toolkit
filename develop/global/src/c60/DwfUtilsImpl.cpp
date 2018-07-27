//
// Created by den on 05.07.18.
//

#include <dgn/ColorType.hpp>
#include "DwfUtilsImpl.h"


typedef std::vector<dgn::SemanticValue> OBJ_SEMANTIC;



DWFString toDwfString(std::wstring str){
	DWFString name;
	name.append(str.c_str());
	return name;
}

std::wstring toStdString(const DWFString &str){
	return std::wstring((const wchar_t *)str, str.chars());
}


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
				if (level == this->section->bottomLevel-2){
					/**читаем новый слой**/
					string str =  string(this->m_string, this->m_length);
					wstring wstr(str.begin(), str.end());
					this->section->layer = &this->section->layers[wstr];
					this->javaHandler->openLayer();
				}
				else{
					if (level == this->section->bottomLevel-1) {
						/**читаем новый объект**/
						string str = string(this->m_string, this->m_length);
						wstring wstr(str.begin(), str.end());
						OBJ_SEMANTIC semantic = this->section->layer->semantics[wstr];
						vector<wstring> values;
						for(auto const &semv : semantic){
							values.push_back(semv.propValue);
						}
						this->javaHandler->openNode(values);
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
				if (level == this->section->bottomLevel-3){
					/*прочитали слой*/
					javaHandler->closeLayer();
				}
				else{
					if (level == this->section->bottomLevel-2){
						/*прочитали объект*/
						javaHandler->closeNode();
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
			this->javaHandler->handleMesh(this->toDgnMesh());
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
				if (level == this->section->bottomLevel+1){
					/*m_mask=7; m_channels=3; m_diffuse и m_specular*/
					dwfColor = new dgn::DwfColor(this->m_diffuse.m_rgb[0], this->m_diffuse.m_rgb[1], this->m_diffuse.m_rgb[2], 0);
					this->javaHandler->handleColor(dgn::ColorType::DIFFUSE, *dwfColor);
					dwfColor = new dgn::DwfColor(this->m_specular.m_rgb[0], this->m_specular.m_rgb[1], this->m_specular.m_rgb[2], 0);
					this->javaHandler->handleColor(dgn::ColorType::SPECULAR, *dwfColor);
				}
				else{
					if (level == this->section->bottomLevel+2){
						/*m_mask=7; m_channels=1; m_diffuse*/
						//dwfColor = new dgn::DwfColor(this->m_diffuse.m_rgb[0], this->m_diffuse.m_rgb[1], this->m_diffuse.m_rgb[2], 0);
						//this->javaHandler->handleColor(dgn::ColorType::DIFFUSE, *dwfColor);
					}
				}
			}
			return eStatus;
		}
};

class MatrixHandler : public TK_Matrix {
protected:
		std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler;
public:
		MatrixHandler(std::shared_ptr<dgn::DwfLayersFillStreamHandler> javaHandler) : TK_Matrix(0) {
			this->javaHandler = javaHandler;
		}

		virtual ~MatrixHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &parser) {
			//TK_Status status = TK_Matrix::Execute(parser);
			//this->javaHandler->handleMesh(this->toDgnMesh());
			return TK_Normal;
		}
};




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

void c60::LayerDWF::SetSemantic(DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst){
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
}

//, std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler
void c60::SectionDWF::SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler){
//**считаем кол-во объектов**//
//void SetCountObject(int32_t level, int32_t layerLevel, int32_t &countLayer, int32_t &countOb, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst){
//void SetLayers(int32_t level, int32_t layerLevel, int32_t &countLayer, int32_t &countOb, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst){
	int32_t currentlevel = level + 1;
	if (currentlevel < this->bottomLevel - 2) {
		DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
		if (piChildren) {
			for (; piChildren->valid(); piChildren->next()) {
				this->SetLayers(currentlevel, pDef, piChildren->value(), handler);
			}
			DWFCORE_FREE_OBJECT(piChildren);
		}
	}
	else {
		if (currentlevel == this->bottomLevel - 2) {
			this->layer = NULL;
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
					if (piChildren->valid()) {
						this->layer->SetFields(pDef, piChildren->value());
						for (; piChildren->valid(); piChildren->next()) {
							this->layer->SetSemantic(pDef, piChildren->value());
						}
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
	}

}

//void GetDWFObjectDefinition(int32_t level, int32_t &bottomLevel, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> &map_semantics){
//void GetDWFObjectDefinition(int32_t level, int32_t &bottomLevel, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> &map_semantics){
//void SetbottomLevel(int32_t level, int32_t &bottomLevel, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> semantics){
void c60::SectionDWF::SetbottomLevel(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst) {
	int32_t currentlevel = level + 1;
	if (currentlevel > this->bottomLevel)
		this->bottomLevel = currentlevel;

	//DWFProperty *pProp = NULL;
	//OBJ_SEMANTIC semantic;
	//DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	//DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	/*
	wstring name_id = L"_name";
	wstring val_id = L"Geometry";
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			wstring name = toStdString(pProp->name());
			wstring value = toStdString(pProp->value());
			if (name.compare(name_id) == 0){
				if (value.compare(val_id) == 0)
					wcout << L"Level geometry: "<< level << endl;
			}
			semantic.push_back(dgn::SemanticValue(name, value));
			semantic.push_back(dgn::SemanticValue(toStdString(pProp->name()), toStdString(pProp->value())));
		}

		DWFCORE_FREE_OBJECT(piProp);
	}
	this->semantics[toStdString(pInst->object())] = semantic;
	 */
	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	//int32_t result = level + 1;
	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			//GetDWFObjectDefinition(currentlevel, bottomLevel, pDef, piChildren->value(), map_semantics);
			SetbottomLevel(currentlevel, pDef, piChildren->value());
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}

}

void c60::DwfUtilsImpl::readTreeStructure(const std::shared_ptr<dgn::DwfLayerStructureStreamHandler> &handler) {

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
			DWFDefinedObjectInstance::tList rRootInstances;
			DWFResourceContainer::ResourceIterator *piObjDefs = pSection->findResourcesByRole(DWFXML::kzRole_ObjectDefinition);
			if (piObjDefs && piObjDefs->valid()) {
				DWFObjectDefinition *pDef = pSection->getObjectDefinition();
				//this->section->pDef = pSection->getObjectDefinition();
				if (pDef) {
					pDef->getRootInstances(rRootInstances);

					int32_t level = 0;
					this->section->bottomLevel = 0;
					//int32_t bottomLevel = 0;
					DWFDefinedObjectInstance *pInst = NULL;
					DWFDefinedObjectInstance::tList::const_iterator iInst = rRootInstances.begin();
					for (; iInst != rRootInstances.end(); iInst++) {
						pInst = *iInst;
						//GetDWFObjectDefinition(level, this->bottomLevel, pDef, pInst, this->semantics);
						this->section->SetbottomLevel(level, pDef, pInst);
						this->section->SetLayers(level, pDef, pInst, handler);
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
						oW3DStreamParser.SetOpcodeHandler(TKE_Modelling_Matrix, new MatrixHandler(handler));

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
