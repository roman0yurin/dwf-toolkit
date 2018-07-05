// TestDWFToolkit.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

using namespace std;
using namespace DWFCore;
using namespace DWFToolkit;


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

typedef std::vector<std::pair<std::wstring, std::wstring>> OBJ_SEMANTIC;

class SegmentOpcodeHandler : public TK_Open_Segment {
protected:
		 std::map<std::wstring, OBJ_SEMANTIC> &semantic;
public:
		SegmentOpcodeHandler(std::map<std::wstring, OBJ_SEMANTIC> &semantic) : TK_Open_Segment(), semantic(semantic) {
		}

		virtual ~SegmentOpcodeHandler() { ; }

		TK_Status Execute(BStreamFileToolkit &rW3DParser) {
			TK_Status eStatus = TK_Open_Segment::Execute(rW3DParser);
			if (eStatus == TK_Normal) {
				wcout << L"\tУровень сегмента: " << rW3DParser.getNestingLevel() << endl;
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

std::wstring toStdString(const DWFString &str){
	return std::wstring((const wchar_t *)str, str.chars());
}

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


int main(int argc, char *argv[]) {
	DWFFile oDWF(argv[1]);
	//DWFFile oDWF(L"C:\\Users\\Den\\Desktop\\Work\\Temp\\АГПЗ_28-04-2017.DWF");
	DWFPackageReader oReader(oDWF);

	DWFPackageReader::tPackageInfo tInfo;
	oReader.getPackageInfo(tInfo);

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

	DWFManifest &rManifest = oReader.getManifest();

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
			/*
			DWFContent* pContent = pContentManager->getContent();
			if (pContent)
			{
				pContent->load();
			}
			*/

			//DWFCORE_FREE_OBJECT(piContents);
		}

		//DWFCORE_FREE_OBJECT(pContentManager);
	}

	DWFManifest::tInterfaceIterator *piInterfaces = rManifest.getInterfaces();
	if (piInterfaces) {
		for (; piInterfaces->valid(); piInterfaces->next()) {
			wcout << L"\tInterface found: " << (const wchar_t *) (piInterfaces->value()->name()) << endl;
		}

		//DWFCORE_FREE_OBJECT(piInterfaces);
	}

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
						oW3DStreamParser.SetOpcodeHandler(TKE_Open_Segment, new SegmentOpcodeHandler(all_semantic));

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
	return 0;
}