// TestDWFToolkit.cpp: определяет точку входа для консольного приложения.
//


#include "stdafx.h"

using namespace std;
using namespace DWFCore;
using namespace DWFToolkit;
using namespace c60;


void GetClassInformation(DWFContent *pContent) {
	DWFClass::tMap::Iterator *piClass = pContent->getClasses();
	if (piClass) {
		for (; piClass->valid(); piClass->next()) {
			DWFClass *pClass = piClass->value();
			if (pClass) {
				wcout << L"Class: " << pClass << endl;

				DWFEntity::tIterator *piEntity = pContent->findEntitiesByClass(pClass);
				if (piEntity) {
					for (; piEntity->valid(); piEntity->next()) {
						DWFEntity *pEntity = piEntity->get();
						wcout << L"SchemaID: " << pEntity << endl;

						DWFCORE_FREE_OBJECT(pEntity);
					}

					DWFCORE_FREE_OBJECT(piEntity);
				}

				DWFCORE_FREE_OBJECT(pClass);
			}
		}

		DWFCORE_FREE_OBJECT(piClass);
	}
}

void GetSharedProperty(DWFContent *pContent) {
	DWFPropertySet::tMap::Iterator *piSet = pContent->getSharedPropertySets();
	if (piSet) {
		for (; piSet->valid(); piSet->next()) {
			DWFPropertySet *pSet = piSet->value();
			if (pSet) {
				wcout << L"PropertySet: " << pSet->getLabel() << L"  id: " << pSet->id() << endl;
				wcout << L"Closed: " << pSet->isClosed() << endl;
				if (pSet->getSchemaID().chars()) {
					wcout << L"SchemaID: " << pSet->getSchemaID() << endl;
				}
				wcout << L"SetID: " << pSet->getSetID() << endl;

				DWFCORE_FREE_OBJECT(pSet);
			}
		}

		DWFCORE_FREE_OBJECT(piSet);
	}
}

void GetContentInfo(DWFContent *pContent) {
	GetSharedProperty(pContent);

	GetClassInformation(pContent);
}

//typedef std::vector<std::pair<std::wstring, std::wstring>> OBJ_SEMANTIC;
/*
class OpenHandler : public TK_Open_Segment {
protected:
		 std::map<std::wstring, OBJ_SEMANTIC> &semantic;
public:
		OpenHandler(std::map<std::wstring, OBJ_SEMANTIC> &semantic) : TK_Open_Segment(), semantic(semantic) {
		}

		virtual ~OpenHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
			TK_Status eStatus = TK_Open_Segment::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				wcout << L"\tNestingLevel : " << rW3DParser.getNestingLevel() << endl;
			}

			string str =  string(this->m_string, this->m_length);
			wstring wstr(str.begin(), str.end());
			auto const result = semantic.find(wstr);
			if (result != semantic.end()){
				OBJ_SEMANTIC sem = result->second;
				for(auto const &pair : sem){
					wcout << pair.first << L" -> " << pair.second << endl;
				}
			}
			wcout << L"\n" << endl;

			return eStatus;
		}
};

class TAGOpcodeHandler : public TK_Tag {
protected:
		std::map<std::wstring, OBJ_SEMANTIC> &semantic;
public:
		TAGOpcodeHandler(std::map<std::wstring, OBJ_SEMANTIC> &semantic) : TK_Tag(), semantic(semantic) {
		}

		virtual ~TAGOpcodeHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
		TK_Status eStatus = TK_Tag::Execute(rW3DParser);
		if (eStatus == TK_Normal) {
		}
		return eStatus;
}
};

class ShellHandler : public TK_Shell {
public:
		ShellHandler() : TK_Shell() { ; }

		virtual ~ShellHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &parser) {
			TK_Status status = TK_Shell::Execute(parser);
			if (status == TK_Normal) {
				//FILE* fp = fopen("C:\\Users\\Den\\Desktop\\Work\\Temp\\coord.txt", "a+");
				int i = 0;
				float x, y, z;
				while (i < this->mp_pointcount) {
					x = this->mp_points[i];
					y = this->mp_points[i + 1];
					z = this->mp_points[i + 2];
					i += 3;
				}
				//fprintf(fp, "%s\n", this->GetString());
				//fclose(fp);
			}
			return status;
		}
};
 */

/*
DWFString toDwfString(std::wstring str){
	DWFString name;
	name.append(str.c_str());
	return name;
}

std::wstring toStdString(const DWFString &str){
	return std::wstring((const wchar_t *)str, str.chars());
}
*/

/*
void GetDWFObjectDefinition(DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, std::map<std::wstring, OBJ_SEMANTIC> &all_semantic){
	const wchar_t *str = (const wchar_t *) pInst->node();
	wcout << L"Object [" << (const wchar_t *) pInst->object() << L"] Node [" << str << L"]  sequence="
				<< pInst->sequence() << endl;
	DWFDefinedObject *ob = NULL;

	DWFProperty *pProp = NULL;
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);

	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	OBJ_SEMANTIC semantic;
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			pProp = piProp->value();
			semantic.push_back(std::make_pair(toStdString(pProp->name()), toStdString(pProp->value())));
			wcout << (const wchar_t *) pProp->name() << L", " << (const wchar_t *) pProp->value() << L"    ["
						<< (const wchar_t *) pProp->category() << L"]" << endl;

		}

		DWFCORE_FREE_OBJECT(piProp);
	}
	all_semantic[toStdString(pInst->object())] = semantic;
	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();

	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			GetDWFObjectDefinition(pDef, piChildren->value(), all_semantic);
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}
}
 */



class OpenHandler : public TK_Open_Segment {
protected:
		c60::SectionDWF *section;
public:
		OpenHandler(c60::SectionDWF *section) : TK_Open_Segment() {
			this->section = section;
		}

		virtual ~OpenHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {

			TK_Status eStatus = TK_Open_Segment::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				int level = rW3DParser.getNestingLevel();
				std::cout << "Open: level=" << level << endl;
				if (level == this->section->levelLayers){
					/**читаем новый слой**/
					string str =  string(this->m_string, this->m_length);
					wstring wstr(str.begin(), str.end());
					this->section->layer = &this->section->layers[wstr];
				}
				else{
					if (this->section->layer != NULL) {
						if (level == this->section->levelLayers - 1) {
							/**читаем новый объект**/
							string str = string(this->m_string, this->m_length);
							wstring wstr(str.begin(), str.end());
							c60::OBJ_SEMANTIC semantic = this->section->layer->semantics[wstr];
							this->section->layer->clearValue();
							this->section->layer->setValues(semantic);
						}
					}
				}
			}

			return eStatus;
		}
};

class CloseHandler: public TK_Close_Segment{
protected:
		c60::SectionDWF *section;
public:
		CloseHandler(c60::SectionDWF *section) : TK_Close_Segment(){
			this->section = section;
		}

		TK_Status Execute(BStreamFileToolkit &rW3DParser){
			TK_Status eStatus = TK_Close_Segment::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				int level = rW3DParser.getNestingLevel();
				std::cout << "Close: level=" << level << endl;
				if (level == this->section->levelLayers - 1){
					/*прочитали слой*/
					this->section->layer = NULL;
				}
				else{
					if (this->section->layer != NULL) {
						if (level == this->section->levelLayers) {
							/*прочитали объект*/
						}
					}
				}
			}
			return eStatus;
		}
};

class ShellHandler : public TK_Shell {
protected:
		c60::SectionDWF *section;
public:
		ShellHandler(c60::SectionDWF *section) : TK_Shell() {
			this->section = section;
		}

		virtual ~ShellHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &parser) {
			TK_Status status = TK_Shell::Execute(parser);
			int level = parser.getNestingLevel();
			std::cout << "pointCount " << this->mp_pointcount << endl;
			std::cout << "item[0].x=" << this->mp_points[0] << endl;
			std::cout << "item[0].y= " << this->mp_points[1] << endl;
			std::cout << "item[0].z= " << this->mp_points[2] << endl;
			std::cout << "########################" << endl;
			/*
			std::cout << "flistlen " << this->m_flistlen << endl;
			std::cout << "pointCount " << this->mp_pointcount << endl;
			std::cout << "faceCount " << this->mp_facecount << endl;
			std::cout << "stage " << this->mp_stage << endl;
			std::cout << "optopcode " << (int)mp_optopcode << endl;
			std::cout << "bitssample " << (int)this->mp_bits_per_sample << endl;
			std::cout << "workspace " << (size_t)this->mp_workspace<< endl;
			std::cout << "w_allocated " << this->mp_workspace_allocated << endl;
			std::cout << "w_used " << this->mp_workspace_used << endl;
			std::cout << "scheme " << (int)this->mp_compression_scheme << endl;
			std::cout << "subop " << (int)this->mp_subop << endl;
			std::cout << "########################" << endl;
			 */
			/*
			for (int i = 0; i < this->mp_pointcount; ++i){
				printf("x= %i", this->);
			}
			 */
			return status;
		}
};

class ColorHandler : public TK_Color {
protected:
		c60::SectionDWF *section;
public:
		ColorHandler(c60::SectionDWF *section) : TK_Color() {
			this->section = section;
		}

		virtual ~ColorHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
			TK_Status eStatus = TK_Color::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				dgn::DwfColor* dwfColor;
				int level = rW3DParser.getNestingLevel();
				if (this->section->layer != NULL) {
					if (level > this->section->levelLayers) {
						/*
						if (level == this->section->layer->bottomLevel + 1) {
							//m_mask=7; m_channels=3; m_diffuse и m_specular
							dwfColor = new dgn::DwfColor(this->m_diffuse.m_rgb[0], this->m_diffuse.m_rgb[1], this->m_diffuse.m_rgb[2],
																					 0);
							dwfColor = new dgn::DwfColor(this->m_specular.m_rgb[0], this->m_specular.m_rgb[1],
																					 this->m_specular.m_rgb[2], 0);
						} else {
							if (level == this->section->layer->bottomLevel + 2) {
								//m_mask=7; m_channels=1; m_diffuse
							}
						}
						 */
					}
				}
			}
			return eStatus;
		}
};

class MatrixHandler : public TK_Matrix {
protected:
		c60::SectionDWF *section;
public:
		MatrixHandler(c60::SectionDWF *section) : TK_Matrix(0) {
			this->section = section;
		}

		virtual ~MatrixHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
			TK_Status status = TK_Matrix::Execute(rW3DParser);
			int level = rW3DParser.getNestingLevel();
			std::cout << "Matrix: level=" << level << endl;
			return status;
		}
};

class TestDWF {
public:
		/**секции файла DWF**/
		std::vector<SectionDWF> sections;
		/**Текущая секция**/
		SectionDWF *section;

		/**Глубина в структуре узлов**/
		int32_t bottomLevel = -1;
		/**Глубина в структуре узлов, на которой находятся слои**/
		int32_t levelLayers = -1;
		/**Глубина в структуре узлов, на которой находятся объекты**/
		int32_t obLevel = -1;

		TestDWF(){};
};


int32_t GetLevelLayers(DWFSection *pSection){
	SectionDWF *section = new SectionDWF();
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

				oW3DStreamParser.SetOpcodeHandler(TKE_Open_Segment, new OpenHandler(section));
				oW3DStreamParser.SetOpcodeHandler(TKE_Close_Segment, new CloseHandler(section));
				oW3DStreamParser.SetOpcodeHandler(TKE_Shell, new ShellHandler(section));
				oW3DStreamParser.SetOpcodeHandler(TKE_Color, new ColorHandler(section));
				oW3DStreamParser.SetOpcodeHandler(TKE_Modelling_Matrix, new MatrixHandler(section));

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
	}

	return section->levelLayers;
}


bool SetLevelLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, TestDWF* ob) {
	int32_t currentlevel = level + 1;
	//if (currentlevel > this->bottomLevel)
	//	this->bottomLevel = currentlevel;

	//DWFProperty *pProp = NULL;
	OBJ_SEMANTIC semantic;
	wstring name = L"";
	wstring value = L"";
	//bool log = true;
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			name = DwfUtilsImpl::toStdString(pProp->name());
			value = DwfUtilsImpl::toStdString(pProp->value());
			/*
			if (name.compare(L"_name") == 0){
				log = false;
				if (value.compare(L"Geometry") == 0){
					if (section->levelLayers == 0) {
						section->levelLayers = currentlevel - 2;
					}
					if (section->layer == NULL){
						section->layer = new LayerDWF();
						section->layer->semanticLevel = currentlevel - 1;
						section->layer->bottomLevel = currentlevel;
					}
				} else{
					if (value.compare(L"Volume") == 0) {
						if (section->levelLayers == 0) {
							section->levelLayers = currentlevel - 4;
						}
						if (section->layer == NULL) {
							section->layer = new LayerDWF();
							section->layer->semanticLevel = currentlevel - 2;
							section->layer->bottomLevel = currentlevel;
						}
					}
				}
			}
			 */
			semantic.push_back(dgn::SemanticValue(name, value));
			//semantic.push_back(dgn::SemanticValue(toStdString(pProp->name()), toStdString(pProp->value())));
		}

		DWFCORE_FREE_OBJECT(piProp);
	}
	//this->semantics[toStdString(pInst->object())] = semantic;
	//if (log){
	//	return false;
	//}

	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	//int32_t result = level + 1;
	if (piChildren) {
		bool log = true;
		for (; piChildren->valid(); piChildren->next()) {
			//GetDWFObjectDefinition(currentlevel, bottomLevel, pDef, piChildren->value(), map_semantics);
			if (!SetLevelLayers(currentlevel, pDef, piChildren->value(), ob))
				return false;
			log = false;
		}
		if (log){
			//if (ob->section)
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}

	/*
	if (section->layer != NULL) {
		if (currentlevel == section->layer->semanticLevel)
			section->layer->semantics[DwfUtilsImpl::toStdString(pInst->object())] = semantic;
		else{
			if (currentlevel == section->levelLayers){
				section->layer->name = value;
				if (!section->layer->SetFields()) return false;

				wstring key = DwfUtilsImpl::toStdString(pInst->object());
				section->layers[key] = *section->layer;
				section->layer = NULL;
			}
		}
	}
	 */

	return true;
}

bool GetSemantic(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, SectionDWF *section) {
	int32_t currentlevel = level + 1;
	OBJ_SEMANTIC semantic;
	DWFPropertyContainer *pInstProps = pDef->getInstanceProperties(*pInst);
	DWFProperty::tMap::Iterator *piProp = pInstProps->getProperties();
	if (piProp) {
		for (; piProp->valid(); piProp->next()) {
			DWFProperty *pProp = piProp->value();
			wstring name = DwfUtilsImpl::toStdString(pProp->name());
			wstring value = DwfUtilsImpl::toStdString(pProp->value());
			//if (name.compare(L"_name") == 0)
			//	name += currentlevel;

			auto ob = section->layer->fields.find(name);
			if (ob == section->layer->fields.end())
				section->layer->fields[name] = L"";
			semantic.push_back(dgn::SemanticValue(name, value));
		}

		DWFCORE_FREE_OBJECT(piProp);
	}

	DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
	if (piChildren) {
		for (; piChildren->valid(); piChildren->next()) {
			if (!GetSemantic(currentlevel, pDef, piChildren->value(), section))
				return false;
		}
		DWFCORE_FREE_OBJECT(piChildren);
	}

	section->layer->semantics[DwfUtilsImpl::toStdString(pInst->object())] = semantic;
	return true;
}

bool SetLayers(int32_t level, DWFObjectDefinition *pDef, DWFDefinedObjectInstance *pInst, SectionDWF *section) {
	int32_t currentlevel = level + 1;
	if (currentlevel < section->levelLayers){
		DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
		if (piChildren) {
			for (; piChildren->valid(); piChildren->next()) {
				if (!SetLayers(currentlevel, pDef, piChildren->value(), section)) {
					DWFCORE_FREE_OBJECT(piChildren);
					return false;
				}
			}

			DWFCORE_FREE_OBJECT(piChildren);
			return true;
		}

		return false;
	}


	if (currentlevel == section->levelLayers) {
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

		section->layer = new LayerDWF();
		section->layer->name = layerName;
		DWFDefinedObjectInstance::tMap::Iterator *piChildren = pInst->resolvedChildren();
		if (piChildren) {
			for (; piChildren->valid(); piChildren->next()) {
				if (!GetSemantic(currentlevel, pDef, piChildren->value(), section))
					return false;
			}
			DWFCORE_FREE_OBJECT(piChildren);
		}

		wstring key = DwfUtilsImpl::toStdString(pInst->object());
		section->layers[key] = *section->layer;
		section->layer = NULL;

		return true;
	}

	return false;
}


int main(int argc, char *argv[]) {
	//DWFFile oDWF(argv[1]);
	//wstring name = L"/home/Documents/Bashny.dwf";
	//DWFFile oDWF(L"/home/den/Documents/TestDWF/Zabor2.dwf");
	DWFFile oDWF(L"/home/den/Documents/TestDWF/Bashny.dwf");
	DWFPackageReader oReader(oDWF);

	DWFPackageReader::tPackageInfo tInfo;
	oReader.getPackageInfo(tInfo);

	if ((tInfo.eType != DWFPackageReader::eDWFPackage) && (tInfo.eType != DWFPackageReader::eDWFXPackage)) {
		// не тот тип файла
		return 1;
	}

	DWFManifest &rManifest = oReader.getManifest();
	TestDWF* ob = new TestDWF();

	DWFSection *pSection = NULL;
	DWFManifest::SectionIterator *piSections = rManifest.getSections();
	if (piSections) {
		for (; piSections->valid(); piSections->next()) {
			pSection = piSections->get();
			pSection->readDescriptor();

			SectionDWF *section = new SectionDWF();
			section->levelLayers = 5;
			/*
			section->levelLayers = GetLevelLayers(pSection);
			if (section->levelLayers < 0)
				return 2;
			 */

			DWFDefinedObjectInstance::tList rRootInstances;
			DWFResourceContainer::ResourceIterator *piObjDefs = pSection->findResourcesByRole(DWFXML::kzRole_ObjectDefinition);
			if (piObjDefs && piObjDefs->valid()) {
				//JLogger::info(L"getObjectDefinition");
				DWFObjectDefinition *pDef = pSection->getObjectDefinition();
				if (pDef) {
					pDef->getRootInstances(rRootInstances);

					int32_t level = 0;
					DWFDefinedObjectInstance *pInst = NULL;
					DWFDefinedObjectInstance::tList::const_iterator iInst = rRootInstances.begin();
					for (; iInst != rRootInstances.end(); iInst++) {
						pInst = *iInst;
						//GetDWFObjectDefinition(level, this->bottomLevel, pDef, pInst, this->semantics);
						//SetLevelLayers(level, pDef, pInst, ob);
						if (!SetLayers(level, pDef, pInst, section))
							return 2;
					}
				}
			}

			/*
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

						oW3DStreamParser.SetOpcodeHandler(TKE_Open_Segment, new OpenHandler(section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Close_Segment, new CloseHandler(section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Shell, new ShellHandler(section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Color, new ColorHandler(section));
						oW3DStreamParser.SetOpcodeHandler(TKE_Modelling_Matrix, new MatrixHandler(section));

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
			}
			 */
		}

		DWFCORE_FREE_OBJECT(piSections);
	}

	/*
	char zBuffer[256] = {0};
	const wchar_t *str;

	if (tInfo.eType != DWFPackageReader::eDWFPackage &&
			tInfo.eType != DWFPackageReader::eDWFXPackage) {
		snprintf(zBuffer, 256, "File is not a DWF package [%s]",
						 (tInfo.eType == DWFPackageReader::eW2DStream) ? "W2D Stream" :
						 (tInfo.eType == DWFPackageReader::eDWFStream) ? "DWF Stream (<6.0)" :
						 (tInfo.eType == DWFPackageReader::eZIPFile) ? "ZIP Archive" : "Unknown");

		wcout << zBuffer << endl;
		exit(0);
	}

	snprintf(zBuffer, 256, "DWF Package version [%0.2f]", (float) (tInfo.nVersion) / 100.0f);
	wcout << zBuffer << endl;

	wcout << L"Reading the manifest..." << endl;

	DWFXDWFProperties *pPr = oReader.getDWFProperties();
	  */

	/*
	snprintf(zBuffer, 256, "\tVersion: %0.2g", rManifest.version());
	wcout << zBuffer << endl;
	wcout << L"\tObject ID: " << (const wchar_t *) rManifest.objectID() << endl;

	DWFContent *pContent = NULL;
	DWFContent::tMap::Iterator *piContents = NULL;
	DWFContentManager *pContentManager = rManifest.getContentManager();
	if (pContentManager) {

		piContents = pContentManager->getContents();
		if (piContents) {
			for (; piContents->valid(); piContents->next()) {
				pContent = piContents->value();
				pContent->load();
				wcout << L"\tInterface found: " << (const wchar_t *) (pContent->id()) << endl;
			}

			//DWFContent* pContent = pContentManager->getContent();
			//if (pContent)
			//{
			//	pContent->load();
			//}

			//DWFCORE_FREE_OBJECT(piContents);
		}

		//DWFCORE_FREE_OBJECT(pContentManager);
	}
	 */

	/*
	DWFManifest::tInterfaceIterator *piInterfaces = rManifest.getInterfaces();
	if (piInterfaces) {
		for (; piInterfaces->valid(); piInterfaces->next()) {
			wcout << L"\tInterface found: " << (const wchar_t *) (piInterfaces->value()->name()) << endl;
		}

		//DWFCORE_FREE_OBJECT(piInterfaces);
	}
	 */

	/*
	DWFProperty *pProperty = NULL;
	DWFProperty::tMap::Iterator *piProperties = rManifest.getProperties();
	if (piProperties) {
		for (; piProperties->valid(); piProperties->next()) {
			pProperty = piProperties->value();
			wcout << L"\tProperty found: " << (const wchar_t *) (pProperty->name());

			if (pProperty->value()) {
				wcout << L" [" << (const wchar_t *) (pProperty->value()) << L"]";
			}

			if (pProperty->category()) {
				wcout << L" [" << (const wchar_t *) (pProperty->category()) << L"]";
			}

			wcout << endl;
			//DWFCORE_FREE_OBJECT(pProperty);
		}

		//DWFCORE_FREE_OBJECT(piProperties);
	}
	 */

	/*
	std::map<std::wstring, OBJ_SEMANTIC> all_semantic;

	DWFSection *pSection = NULL;
	DWFManifest::SectionIterator *piSections = rManifest.getSections();
	if (piSections) {
		for (; piSections->valid(); piSections->next()) {
			pSection = piSections->get();
			pSection->readDescriptor();

			//
			// piSections->key() will also return the section name...
			//
			wcout << L"\tSection found: " << (const wchar_t *) (pSection->name());
			str = (const wchar_t *)(pSection->type());
			if (str) {
				wcout << L" [" << str << L"]";
			}
			str = (const wchar_t *) (pSection->title());
			if (str) {
				wcout << L" [" << str << L"]";
			}
			wcout << endl;

			pContentManager = pSection->getContentManager();
			if (pContentManager) {
				piContents = pContentManager->getContents();
				if (piContents) {
					for (; piContents->valid(); piContents->next()) {
						pContent = piContents->value();
						pContent->load();
						wcout << L"\tInterface found: " << (const wchar_t *) (pContent->id()) << endl;
					};
				}
			}

			DWFContent::tIterator *piContent = pSection->getContentDefinition();
			if (piContent) {
				for (; piContent->valid(); piContent->next()) {
					pContent = piContent->get();
				}
			}

			DWFPropertyContainer *pContainer = NULL;
			DWFDefinedObjectInstance::tList rRootInstances;
			DWFResourceContainer::ResourceIterator *piObjDefs = pSection->findResourcesByRole(
							DWFXML::kzRole_ObjectDefinition);
			if (piObjDefs && piObjDefs->valid()) {
				DWFObjectDefinition *pDef = pSection->getObjectDefinition();
				if (pDef) {
					pDef->getRootInstances(rRootInstances);

					DWFDefinedObjectInstance *pInst = NULL;
					DWFDefinedObjectInstance::tList::const_iterator iInst = rRootInstances.begin();
					for (; iInst != rRootInstances.end(); iInst++) {
						pInst = *iInst;
						GetDWFObjectDefinition(pDef, pInst, all_semantic);
					}
				}
			}

			DWFResourceContainer::ResourceKVIterator *piResource = pSection->getResourcesByRole();
			if (piResource) {
				for (; piResource->valid(); piResource->next()) {
					DWFResource *pResource = piResource->value();
					const wchar_t *str = (const wchar_t *) (pResource->title());
					if (str)
						wcout << L"\tDWFResource_title: " << str << endl;
					str = (const wchar_t *) (pResource->role());
					if (str)
						wcout << L"\tDWFResource_role: " << str << endl;
					str = (const wchar_t *) (pResource->mime());
					if (str)
						wcout << L"\tDWFResource_mime: " << str << endl;
					str = (const wchar_t *) (pResource->objectID());
					if (str)
						wcout << L"\tDWFResource_objectID: " << str << endl;

					//size_t propertyCount = pResource->getPropertyCount();
					DWFGraphicResource *pW3D = dynamic_cast<DWFGraphicResource *>(pResource);
					if (pW3D) {
						//
						// get the data stream
						//
						DWFCore::DWFInputStream *pW3DStream = pW3D->getInputStream();

						//
						// Create the HSF toolkit object that does the stream I/O
						//
						BStreamFileToolkit oW3DStreamParser;

						//
						// For this sample, we are interested in those op-codes that might contain
						// some interesting text.  Here is where we hook these handlers.
						// Also note that the parser object will delete this object on it's own destruction
						//
						//oW3DStreamParser.SetOpcodeHandler(TKE_Start_User_Data, new StartUserDataHandler);
						//oW3DStreamParser.SetOpcodeHandler(TKE_Stop_User_Data, new StartUserDataHandler);

						//oW3DStreamParser.SetOpcodeHandler(TKE_Shell, new ShellHandler);

						//oW3DStreamParser.SetOpcodeHandler(TKE_Comment, new CommentHandler);
						oW3DStreamParser.SetOpcodeHandler(TKE_Tag, new TAGOpcodeHandler(all_semantic));
						//oW3DStreamParser.SetOpcodeHandler(TKE_Text_With_Encoding, new TextWithEncodingOpcodeHandler);
						oW3DStreamParser.SetOpcodeHandler(TKE_Open_Segment, new OpenHandler(all_semantic));

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

						wcout << L"OK\n" << endl;
					} else {
						DWFObjectDefinitionResource *pDefinitionResource = dynamic_cast<DWFObjectDefinitionResource *>(pResource);
						if (pDefinitionResource) {
							wcout << L"OK\n" << endl;
						} else {
							DWFFontResource *pFontResource = dynamic_cast<DWFFontResource *>(pResource);
							if (pFontResource) {
								wcout << L"OK\n" << endl;
							}
						}
					}
				}
			}

			DWFEModelSection *peSection = dynamic_cast<DWFEModelSection *>(pSection);
			if (peSection) {

				DWFInterface *pInterface = peSection->buildInterface();
				wcout << L"\tEModel_Name: " << pInterface->kzEModel_Name << endl;
				wcout << L"\tEModel_HRef: " << pInterface->kzEModel_HRef << endl;
				wcout << L"\tEModel_ID: " << pInterface->kzEModel_ID << endl;
				DWFCORE_FREE_OBJECT(pInterface);


			}
		}

		DWFCORE_FREE_OBJECT(piSections);
	}
	 */

	/*
	piSections = rManifest.getGlobalSections();
	if (piSections) {
		wcout << "Reading section descriptors..." << endl << endl;

		for (; piSections->valid(); piSections->next()) {
			pSection = piSections->get();

			pSection->readDescriptor();

			DWFGlobalSection *pGlobal = dynamic_cast<DWFGlobalSection *>(pSection);

			if (pGlobal == NULL) {
				wcout << L"\t[OK] (" << pSection->order() << L") v" << pSection->version();

				if (pSection->name()) {
					wcout << L" [" << (const wchar_t *) (pSection->name()) << L"]";
				}

				if (pSection->objectID()) {
					wcout << L" [" << (const wchar_t *) (pSection->objectID()) << L"]";
				}

				wcout << L" [" << pSection->order() << L"]";

				wcout << endl;
			} else {
				wcout << L"\t[OK] (Global Section)" << endl;

				const DWFBookmark *pRoot = pGlobal->bookmark();
				if (pRoot) {
					wcout << L"\tDumping bookmarks..." << endl;
				}
			}
		}

		DWFCORE_FREE_OBJECT(piSections);
	}

	wcout << L"OK\n";
	 */

	return 0;
}