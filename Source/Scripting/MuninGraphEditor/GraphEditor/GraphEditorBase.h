#pragma once
#include <algorithm>
#include <fstream>
#include <memory>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "GraphEditorUtils.h"
#include "imgui.h"
#include "../ImGuiNodeEditor/imgui_node_editor_internal.h"
#include "../ImGuiNodeEditor/imgui_node_editor.h"
#include "imgui_stdlib.h"
#include "MuninGraph.h"
#include "Commands/Command.h"
#include "Commands/EdgeCommands.h"
#include "Commands/NodeCommands.h"
#include "Fonts/IconsFontAwesome6.h"
#include "NodeGraph/NodeGraphCommon.h"

namespace ImNodeEd = ax::NodeEditor;

struct GraphEditorSettingsBase
{
	ImFont* NodeTitleFont = nullptr;
	ImFont* NodeBodyFont = nullptr;
	ImFont* EditorTokenFont = nullptr;
	ImFont* EditorFont = nullptr;

	ImFont* FontAwesomeRegular = nullptr;
	ImFont* FontAwesomeSolid = nullptr;

	bool AutoSave = true;

	GraphEditorSettingsBase();
	virtual ~GraphEditorSettingsBase();
};

struct GraphEditorContextMenuItem
{
	std::string Title;
	const RegisteredNodeClass* Value;
	std::string Tag;
};

struct GraphEditorContextMenuCategory
{
	std::string Title;
	std::vector<GraphEditorContextMenuItem> Items;
	std::vector<GraphEditorContextMenuCategory> SubCatagories;
};

struct GraphEditorStateBase
{
	virtual ~GraphEditorStateBase();

	struct BackgroundContextMenuInfo
	{
		std::unordered_map<std::string, GraphEditorContextMenuCategory> Categories;
		std::vector<std::string> CategoryNamesList;
		std::vector<GraphEditorContextMenuCategory> AllCategories;
		std::unordered_map<std::string, size_t> CategoryNameToIndex;
		bool MenuOpen = false;
		ImVec2 MenuPosition = ImVec2(0, 0);
	} BackgroundContextMenu;

	struct ContextSearchInfo
	{
		bool FieldChanged = false;
		bool SetKeyboardFocus = false;
		std::string Value;
		struct SearchMenuItem
		{
			GraphEditorContextMenuItem Item;
			float Rank = FLT_MAX;
		};
		std::vector<SearchMenuItem> SearchResults;
	} ContextSearch;

	struct InternalInfo
	{
		bool RefreshNodePositions = true;
	} Layout;

	size_t PinContextMenuId = 0;
};

template<typename GraphClass, typename GraphNodeClass, typename GraphEdgeClass, typename GraphPinClass, typename GraphSchemaClass>
class GraphEditorBase
{
public:
	GraphEditorBase(GraphEditorSettingsBase* aSettings, GraphEditorStateBase* aState, GraphClass* aGraph)
		: mySettings(aSettings), myEditorState(aState)
	{
		// This is very buggy so turning it off.
		myNodeEditorConfig.SettingsFile = nullptr;
		myNodeEditorConfig.CanvasSizeMode = ax::NodeEditor::CanvasSizeMode::CenterOnly;

		myEditorContext = ImNodeEd::CreateEditor(&myNodeEditorConfig);
		myGraph = aGraph;		
		mySchema = new GraphSchemaClass(myGraph);
		GraphEditorStateBase::BackgroundContextMenuInfo& contextMenu = myEditorState->BackgroundContextMenu;

		const std::vector<const RegisteredNodeClass*> schemaNodeClasses = MuninGraph::Get().GetSupportedNodeClasses(typeid(GraphSchemaClass));
		for(const auto nodeClass : schemaNodeClasses)
		{
			if(nodeClass->HasFlag(CF_Internal))
				continue;

			if(nodeClass->ClassDefaultObject)
			{
				const std::string& category = nodeClass->Category;
				auto categoryHierarchy = explode_string(category, "|");
				// Categories can have similar part names.

				// If the category doesn't exist, create it.
				if (!contextMenu.CategoryNameToIndex.contains(categoryHierarchy.front()))
				{
					GraphEditorContextMenuCategory newCategory;
					newCategory.Title = categoryHierarchy.front();
					contextMenu.CategoryNameToIndex.emplace(newCategory.Title, contextMenu.AllCategories.size());
					contextMenu.AllCategories.emplace_back(std::move(newCategory));
				}

				const size_t categoryIndex = contextMenu.CategoryNameToIndex.at(categoryHierarchy.front());
				GraphEditorContextMenuCategory& menuCategory = contextMenu.AllCategories[categoryIndex];
				GraphEditorContextMenuItem newItem;
				newItem.Title = nodeClass->Name;
				newItem.Value = nodeClass;
				categoryHierarchy.erase(categoryHierarchy.begin());
				addItemToCategory(menuCategory, categoryHierarchy, newItem);
			}
		}

		std::ranges::sort(contextMenu.AllCategories, std::ranges::less{}, &GraphEditorContextMenuCategory::Title);
		for (auto& category : contextMenu.AllCategories)
		{			
			std::ranges::sort(category.Items, std::ranges::less{}, &GraphEditorContextMenuItem::Title);
			for(auto& subcategory : category.SubCatagories)
			{
				std::ranges::sort(subcategory.Items, std::ranges::less{}, &GraphEditorContextMenuItem::Title);
			}
		}
	}

	virtual ~GraphEditorBase()
	{
		if(myEditorContext)
		{
			ImNodeEd::DestroyEditor(myEditorContext);
		}
		myEditorState = nullptr;
		mySettings = nullptr;
	}

	//virtual void Init(GraphClass* aGraph);
	virtual void Render();

	virtual void Update(float aDeltaTime);
protected:

	typedef GraphEditorBase Base;
	GraphClass* myGraph = nullptr;
	GraphSchemaClass* mySchema = nullptr;
	ImNodeEd::EditorContext* myEditorContext = nullptr;
	ImNodeEd::Config myNodeEditorConfig;
	GraphEditorSettingsBase* mySettings;
	GraphEditorStateBase* myEditorState;
	std::vector<std::shared_ptr<Command>> myCommands;
	int myCommandIndex;


	float JaroWinklerDistance(std::string_view s1, std::string_view s2, bool caseSensitive = false)
	{
		float m = 0;
		size_t low, high;
		size_t k = 0, t = 0;

		if(s1.empty() || s2.empty())
			return 0;

		std::string str1(s1);
		std::string str2(s2);

		if(!caseSensitive)
		{
			std::ranges::transform(str1, str1.begin(), tolower);
			std::ranges::transform(str2, str2.begin(), tolower);
		}

		if(str1 == str2)
			return 1;

		const size_t range = std::max<size_t>(s1.length(), s2.length()) / 2 - 1;

		std::vector<size_t> str1Matches;
		str1Matches.resize(str1.length());
		std::vector<size_t> str2Matches;
		str2Matches.resize(str2.length());

		for(size_t i = 0; i < str1.length(); i++)
		{
			if(i >= range)
			{
				low = i - range;
			}
			else
			{
				low = 0;
			}

			if(i + range <= str2.length() - 1)
			{
				high = i + range;
			}
			else
			{
				high = s2.length() - 1;
			}

			for(size_t j = low; j <= high; j++)
			{
				if(str1Matches[i] != 1 && str2Matches[j] != 1 && str1[i] == str2[j])
				{
					m += 1;
					str1Matches[i] = 1;
					str2Matches[j] = 1;
					break;
				}
			}
		}

		if(m == 0)
		{
			return 0;
		}

		for(size_t i = 0; i < str1.length(); i++)
		{
			if(str1Matches[i] == 1)
			{
				size_t j = 0;
				for(j = k; j < str2.length(); j++)
				{
					if(str2Matches[j] == 1)
					{
						k = j + 1;
						break;
					}
				}

				if(str1[i] != str2[j])
				{
					t += 1;
				}
			}
		}

		float w = (m / str1.length() + m / str2.length() + (m - t / 2) / m) / 3;
		if(w > 0.7f)
		{
			float p = 0.1f;
			size_t l = 0;
			while(str1[l] == str2[l] && l < 4)
			{
				l += 1;
			}

			w += l * p * (1 - w);
		}

		return w;
	}

	//~ Render Functions
	// This is responsible for rendering the actual window we live in.
	// It calls all the other rendering functions to render each specific part.
	virtual void RenderWindow();

	// If you have a Toolbar in your window you render it here.
	// By default it renders Save/Load buttons.
	virtual void RenderToolbar();
	virtual void RenderBackgroundContextMenu();
	void RenderBackgroundContextMenuHierarchy(
		const GraphEditorContextMenuCategory& aCategory);
	virtual void RenderUserBackgroundContextMenu();
	virtual void RenderPinContextMenu();
	virtual void RenderUserPinContextMenu();

	virtual void RenderEditorStyle();

	virtual void RenderNode(const std::shared_ptr<GraphNodeClass>& aNode);
	virtual void RenderNodeHeader(const std::shared_ptr<GraphNodeClass>& aNode, const ImVec2& aNodeHeaderRect);
	virtual void RenderNodeStyle(const std::shared_ptr<GraphNodeClass>& aNode, const ImRect& aNodeHeaderRect, const ImRect& aNodeBodyRect);

	virtual void RenderPin(const GraphPinClass& aPin);
	virtual void RenderPinIcon(const GraphPinClass& aPin, const ImRect& aPinRect);

	virtual void RenderEdge(const GraphEdgeClass& anEdge);	
	virtual void RenderGraphEditor();
	//~

	virtual void HandleImNodeCreateNode();
	virtual void HandleImNodeDeleteNode();
	virtual void HandleUserTransaction();
	virtual std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem> HandleContextMenuSearch(std::string_view aSearchQuery);
	virtual void HandleBackgroundContextMenuItemClicked(const GraphEditorContextMenuItem& aItem);

	virtual std::string_view GetEditorTitle() const;
	virtual std::string_view GetEditorToken() const;

	std::vector<std::string> explode_string(const std::string& aString, const std::string& aDelimiter);
	
	void addItemToCategory(GraphEditorContextMenuCategory& category, const std::vector<std::string>& categoryHierarchy, const GraphEditorContextMenuItem& item);

	void Undo();
	void Redo();

	void AddCommand(std::shared_ptr<Command> aCommand); // Adds command to list and handel removal of redo steps
	void CleanCommands();
};

// Helps improve readability of this file :P
#define GraphEditorMethod(...) template <typename GraphClass, typename GraphNodeClass, typename GraphEdgeClass, typename GraphPinClass, typename GraphSchemaClass> \
	__VA_ARGS__ GraphEditorBase<GraphClass, GraphNodeClass, GraphEdgeClass, GraphPinClass, GraphSchemaClass>

GraphEditorMethod(void)::Render()
{
	ImGui::PushFont(mySettings->EditorFont);
	RenderWindow();
	ImGui::PopFont();
}

GraphEditorMethod(void)::HandleImNodeCreateNode()
{
	if (ImNodeEd::BeginCreate())
	{
		ImNodeEd::PinId startLinkId, endLinkId;

		// This returns True constantly while trying to create a link, even before we've selected an end pin.
		if (ImNodeEd::QueryNewLink(&startLinkId, &endLinkId))
		{
			if (startLinkId && endLinkId)
			{

				std::string canCreateEdgeFeedback;
				if (!mySchema->CanCreateEdge(startLinkId.Get(), endLinkId.Get(), canCreateEdgeFeedback))
				{
					ImNodeEd::RejectNewItem(ImColor(255, 0, 0, 255));
				}

				// This is true if we've made a link between two pins. I.e. when we release LMB to make a link.
				if (ImNodeEd::AcceptNewItem())
				{
					int edgeId = mySchema->CreateEdge(startLinkId.Get(), endLinkId.Get());
					AddCommand(std::make_shared<AddEdgeCommand>(startLinkId.Get(), endLinkId.Get(), edgeId, mySchema));
					//Add edge to undo redo
				}
			}
		}
	}
	ImNodeEd::EndCreate();
}

GraphEditorMethod(void)::HandleImNodeDeleteNode()
{
	if (ImNodeEd::BeginDelete())
	{
		ImNodeEd::LinkId deletedLinkId;
		while (ImNodeEd::QueryDeletedLink(&deletedLinkId))
		{
			if (ImNodeEd::AcceptDeletedItem())
			{
				auto edge = myGraph->GetEdgeFromId(deletedLinkId.Get());
				
				AddCommand(std::make_shared<RemoveEdgeCommand>(edge.FromId,edge.ToId,deletedLinkId.Get(), mySchema));
				mySchema->RemoveEdge(deletedLinkId.Get());
				//add to undo redo
			}
		}

		ImNodeEd::NodeId deletedNodeId = 0;
		while (ImNodeEd::QueryDeletedNode(&deletedNodeId))
		{
			if (ImNodeEd::AcceptDeletedItem())
			{
				//Get Node info needed for command
				auto node = mySchema->GetNodes().at(deletedNodeId.Get());
				RegisteredNodeClass type = MuninGraph::Get().GetNodeClass(AsObjectUIDSharedPtr(node)->GetTypeName());
				float x,y,z;
				node->GetNodePosition(x, y, z);
				for (auto& [id, pin] : node->GetPins()) 
				{
					for (size_t edgeId : pin.GetEdges()) 
					{
				        auto edge = myGraph->GetEdgeFromId(edgeId);
				        AddCommand(std::make_shared<RemoveEdgeCommand>(edge.FromId, edge.ToId, deletedLinkId.Get(), mySchema));
				    }
				}
				if(mySchema->RemoveNode(deletedNodeId.Get()))
				{
					//add to undo redo
					AddCommand(std::make_shared<RemoveNodeCommand>(type,deletedNodeId.Get(), mySchema, x,y,z));
				}
			}
		}
	}
	ImNodeEd::EndDelete();
}

GraphEditorMethod(void)::HandleUserTransaction()
{
}

GraphEditorMethod(std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem>)::HandleContextMenuSearch(std::string_view aSearchQuery)
{
	std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem> results;
	const std::vector<const RegisteredNodeClass*> schemaNodeClasses = MuninGraph::Get().GetSupportedNodeClasses(typeid(GraphSchemaClass));
	for(const auto& nodeClass : schemaNodeClasses)
	{
		if(nodeClass->HasFlag(CF_Internal))
			continue;

		const float rank = JaroWinklerDistance(nodeClass->Name, aSearchQuery);
		GraphEditorContextMenuItem foundItem;
		foundItem.Title = nodeClass->Name;
		foundItem.Value = nodeClass;
		results.emplace_back(
			foundItem, rank
		);
	}

	std::ranges::sort(results, [](const GraphEditorStateBase::ContextSearchInfo::SearchMenuItem& A, const GraphEditorStateBase::ContextSearchInfo::SearchMenuItem& B)
	{
		return A.Rank > B.Rank;
	});

	return results;
}

GraphEditorMethod(void)::HandleBackgroundContextMenuItemClicked(const GraphEditorContextMenuItem& aItem)
{
	const ImVec2 mousePos = ImNodeEd::ScreenToCanvas(ImGui::GetMousePos());
	if(const std::shared_ptr<GraphNodeClass> newNode = mySchema->CreateNode(*aItem.Value))
	{
		auto uidNewNode = AsObjectUIDSharedPtr(newNode);
		ImNodeEd::SetNodePosition(uidNewNode->GetUID(), mousePos);
		AddCommand(std::make_shared<CreateNodeCommand>(*aItem.Value, uidNewNode->GetUID(), mySchema));
		//add to undo redo
	}
}

GraphEditorMethod(std::string_view)::GetEditorTitle() const
{
	return "MuninGraph Editor";
}

GraphEditorMethod(std::string_view)::GetEditorToken() const
{
	return "Graph";
}

template <typename GraphClass, typename GraphNodeClass, typename GraphEdgeClass, typename GraphPinClass, typename
	GraphSchemaClass>
std::vector<std::string> GraphEditorBase<GraphClass, GraphNodeClass, GraphEdgeClass, GraphPinClass, GraphSchemaClass>::
explode_string(const std::string& aString, const std::string& aDelimiter)
{
	std::vector<std::string> result;

	size_t start = 0ULL;
	size_t end = aString.find(aDelimiter);

	while(end != std::string::npos)
	{
		result.push_back(aString.substr(start, end - start));
		start = end + aDelimiter.length();
		end = aString.find(aDelimiter, start);
	}

	result.push_back(aString.substr(start));

	return result;
}

GraphEditorMethod(void)::addItemToCategory(GraphEditorContextMenuCategory& category, const std::vector<std::string>& categoryHierarchy,const GraphEditorContextMenuItem& item)
{
    // Base case: If there's no more hierarchy, add the item to this category
    if (categoryHierarchy.empty())
    {
        category.Items.push_back(item);
        return;
    }

    // Recursive case: If there's more hierarchy, find or create the subcategory
    const std::string& subCategoryTitle = categoryHierarchy.front();
    auto subCategoryIt = std::find_if(category.SubCatagories.begin(), category.SubCatagories.end(),
                                      [&subCategoryTitle](const GraphEditorContextMenuCategory& subCategory) {
                                          return subCategory.Title == subCategoryTitle;
                                      });

    // If subcategory is not found, create it
    if (subCategoryIt == category.SubCatagories.end())
    {
        GraphEditorContextMenuCategory newSubCategory;
        newSubCategory.Title = subCategoryTitle;
        category.SubCatagories.push_back(std::move(newSubCategory));
        subCategoryIt = std::prev(category.SubCatagories.end());
    }

    // Recursively call for the next level in the hierarchy (if any)
    addItemToCategory(*subCategoryIt, {categoryHierarchy.begin() + 1, categoryHierarchy.end()}, item);
}

GraphEditorMethod(void)::Undo()
{
	if(myCommandIndex>-1)
	{
		if(myCommands.at(myCommandIndex)->Undo())
		{
			myCommandIndex--;
		}
	}
	
	myEditorState->Layout.RefreshNodePositions = true;
}

GraphEditorMethod(void)::Redo()
{
	if(myCommandIndex+1<myCommands.size()|| myCommandIndex < 0)
	{
		if(myCommands.at(myCommandIndex+1)->Redo())
		{
			myCommandIndex++;
		}
		if(myCommandIndex== myCommands.size())
		{
			myCommandIndex--;
		}
	}
	myEditorState->Layout.RefreshNodePositions = true;
}

GraphEditorMethod(void)::AddCommand(std::shared_ptr<Command> aCommand)
{
	if(myCommandIndex+1 < myCommands.size())
	{
		myCommands.erase(myCommands.begin()+myCommandIndex+1, myCommands.end());
		myCommandIndex = (int)myCommands.size()-1;
	}
	myCommandIndex++;
	myCommands.push_back(aCommand);
	if(myCommandIndex== myCommands.size())
	{
		myCommandIndex--;
	}
}

GraphEditorMethod(void)::CleanCommands()
{
	if(myCommandIndex <0)
	{
		return;
	}
	if(!myCommands.empty() && myCommands.at(myCommandIndex)->Clean())
	{
		myCommands.erase(myCommands.begin()+myCommandIndex);
	}
}

GraphEditorMethod(void)::Update(
	float aDeltaTime)
{
	aDeltaTime;
	CleanCommands();
}

GraphEditorMethod(void)::RenderGraphEditor()
{
	ImNodeEd::PushStyleVar(ax::NodeEditor::StyleVar_SnapLinkToPinDir, 1.0f);
	ImNodeEd::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 100.0f);

	for(const auto& [id, node] : mySchema->GetNodes())
	{
		if(myEditorState->Layout.RefreshNodePositions)
		{
			float X, Y, Z;
			node->GetNodePosition(X, Y, Z);
			ImNodeEd::SetNodePosition(id, {X, Y});
			ImNodeEd::SetNodeZPosition(id, Z);
		}

		RenderNode(node);
		float X, Y, Z;
		node->GetNodePosition(X, Y, Z);
		// Sync node position with ImNodeEd node position in case someone moved it in editor.
		
		ImVec2 newNodePos = ImNodeEd::GetNodePosition(id);
		float newNodeZ = ImNodeEd::GetNodeZPosition(id);
		if(X != newNodePos.x || Y != newNodePos.y || Z != newNodeZ)
		{
			node->SetNodePosition(newNodePos.x, newNodePos.y, newNodeZ);
		}
		else
		{
			if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				node->ResetMoved();
			}
		}
		
		if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && node->WasMoved())
		{
			node->GetPrevNodePosition(X,Y,Z);
			AddCommand(std::make_shared<MoveNodeCommand>(node,X,Y,Z,newNodePos.x, newNodePos.y, newNodeZ));
			node->ResetMoved();
			node->ResetLastFrameMove();
		}
		
	}

	myEditorState->Layout.RefreshNodePositions = false;

	for(const auto& [id, edge] : mySchema->GetEdges())
	{
		RenderEdge(edge);
	}

	ImNodeEd::PopStyleVar(2);
}

GraphEditorMethod(void)::RenderWindow()
{
	ImNodeEd::SetCurrentEditor(myEditorContext);
	ImGui::SetNextWindowSize({ 1280, 720}, ImGuiCond_FirstUseEver);
	if (ImGui::Begin(GetEditorTitle().data(), nullptr, ImGuiWindowFlags_MenuBar))
	{
		//ImGui::ShowDemoWindow();
		RenderToolbar();
		ImNodeEd::Begin("MuninGraphEditor");
		
		ImNodeEd::Suspend();
		ImVec2 tokenPos = ImGui::GetWindowContentRegionMax();
		ImGui::PushFont(mySettings->EditorTokenFont);
		const ImVec2 tokenSize = ImGui::CalcTextSize(GetEditorToken().data());
		tokenPos = { tokenPos.x - tokenSize.x - 24, tokenPos.y - tokenSize.y - 12 };

		ImGui::SetCursorPos(tokenPos);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 128));
		ImGui::TextUnformatted(GetEditorToken().data());
		ImGui::PopStyleColor();
		ImGui::PopFont();

		ImNodeEd::Resume();

		RenderGraphEditor();

		HandleImNodeCreateNode();
		HandleImNodeDeleteNode();
		HandleUserTransaction();

		ImNodeEd::Suspend();

		if(ImNodeEd::ShowBackgroundContextMenu())
		{
			ImGui::OpenPopup("BackgroundContextMenu");
		}

		RenderBackgroundContextMenu();

		for(auto& pinId : mySchema->GetPins() | std::views::keys)
		{
			ImNodeEd::PinId id = pinId;
			// ShowPinContextMenu returns true for all pins on the same node
			// regardless of which one you actually clicked on. Therefore you
			// need to check if it's the pin that is being hovered that you
			// clicked on to get the correct context.
			if(ImNodeEd::GetHoveredPin() == id)
			{
				if(ImNodeEd::ShowPinContextMenu(&id))
				{
					myEditorState->PinContextMenuId = pinId;
					ImGui::OpenPopup("PinContextMenu");
					break; // Only one popup please xP
				}
			}
		}

		RenderPinContextMenu();

		ImNodeEd::Resume();
		RenderEditorStyle();
		ImNodeEd::End();
	}
	ImGui::End();
	ImNodeEd::SetCurrentEditor(nullptr);
}

GraphEditorMethod(void)::RenderToolbar()
{
	// No default toolbar to render.
}

GraphEditorMethod(void)::RenderBackgroundContextMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("BackgroundContextMenu"))
	{
		myEditorState->ContextSearch.FieldChanged = ImGui::InputText("##ctxtMenuSearch", &myEditorState->ContextSearch.Value);
		if(myEditorState->ContextSearch.SetKeyboardFocus)
		{
			ImGui::SetKeyboardFocusHere(0);
			myEditorState->ContextSearch.SetKeyboardFocus = false;
		}
		
		if(myEditorState->ContextSearch.Value.empty())
		{
			for(const auto& category : myEditorState->BackgroundContextMenu.AllCategories)
			{
				RenderBackgroundContextMenuHierarchy(category);
				
			}
		}
		else
		{
			if(myEditorState->ContextSearch.FieldChanged)
			{
				myEditorState->ContextSearch.SearchResults.clear();
				myEditorState->ContextSearch.SearchResults = std::move(HandleContextMenuSearch(myEditorState->ContextSearch.Value));
			}

			for(const auto& item : myEditorState->ContextSearch.SearchResults)
			{
				if(ImGui::TreeNodeEx(item.Item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						HandleBackgroundContextMenuItemClicked(item.Item);
						myEditorState->ContextSearch.Value.clear();
						myEditorState->ContextSearch.FieldChanged = false;
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

GraphEditorMethod(void)::RenderBackgroundContextMenuHierarchy(const GraphEditorContextMenuCategory& aCategory)
{
	if (ImGui::TreeNodeEx(aCategory.Title.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
	{
			for(const auto& item : aCategory.Items)
			{
				if(ImGui::TreeNodeEx(item.Title.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
				{
					if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						HandleBackgroundContextMenuItemClicked(item);
						myEditorState->ContextSearch.Value.clear();
						myEditorState->ContextSearch.FieldChanged = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::TreePop();
				}
			}
			for(const auto& subCategory : aCategory.SubCatagories)
			{
				RenderBackgroundContextMenuHierarchy(subCategory);
			}
		ImGui::TreePop();
	}
}

GraphEditorMethod(void)::RenderUserBackgroundContextMenu()
{
	// Nothing by default. Can override this to extend the menu with your own items.
}

GraphEditorMethod(void)::RenderPinContextMenu()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("PinContextMenu"))
	{
		const ScriptGraphPin* pin = mySchema->GetPin(myEditorState->PinContextMenuId);
		ImGui::BeginDisabled(!pin->IsDynamicPin());
		if(ImGui::TreeNodeEx("Delete Pin", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			if(ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				mySchema->MarkDynamicPinForDelete(myEditorState->PinContextMenuId);
				myEditorState->PinContextMenuId = 0;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::TreePop();
		ImGui::EndDisabled();
		ImGui::EndPopup();
	}
	else
	{
		myEditorState->PinContextMenuId = 0;
	}
	ImGui::PopStyleVar();
}

GraphEditorMethod(void)::RenderUserPinContextMenu()
{
	// Nothing by default. Can override this to extend the menu with your own items.
}

GraphEditorMethod(void)::RenderEditorStyle()
{
	// Nothing by default.
}

GraphEditorMethod(void)::RenderNode(const std::shared_ptr<GraphNodeClass>& aNode)
{
	std::vector<const GraphPinClass*> inputPins;
	std::vector<const GraphPinClass*> outputPins;

	ImGui::PushFont(mySettings->NodeTitleFont);
	const ImVec2 headerTextSize = ImGui::CalcTextSize(aNode->GetNodeTitle().data());
	ImGui::PopFont();

	ImVec2 leftMinSize = { 64, 0 };
	ImVec2 rightMinSize = { 64, 0 };

	ImGui::PushFont(mySettings->NodeBodyFont);
	for(const auto& [id, pin] : aNode->GetPins())
	{
		const ImVec2 currentTextSize = ImGui::CalcTextSize(pin.GetLabel().data());

		if (pin.GetPinDirection() == PinDirection::Input)
		{
			if (currentTextSize.x > leftMinSize.x)
				leftMinSize.x = currentTextSize.x;
			if (currentTextSize.y > leftMinSize.y)
				leftMinSize.y = currentTextSize.y;

			inputPins.push_back(&pin);
		}
		else
		{
			if (currentTextSize.x > rightMinSize.x)
				rightMinSize.x = currentTextSize.x;
			if (currentTextSize.y > rightMinSize.y)
				rightMinSize.y = currentTextSize.y;

			outputPins.push_back(&pin);
		}
	}
	ImGui::PopFont();

	if (leftMinSize.x > 0)
		leftMinSize.x += 48;

	if (rightMinSize.x > 0)
		rightMinSize.x += 48;

	//std::sort(inputPins.begin(), inputPins.end(), [](const GraphPinClass* A, const GraphPinClass* B) { return A->GetUID() < B->GetUID(); });
	//std::sort(outputPins.begin(), outputPins.end(), [](const GraphPinClass* A, const GraphPinClass* B) { return A->GetUID() < B->GetUID(); });

	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodeRounding, 3.0f);
	ImNodeEd::PushStyleVar(ImNodeEd::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

	const auto uidAwareNode = AsObjectUIDSharedPtr(aNode);
	const ImNodeEd::NodeId currentImNodeId = uidAwareNode->GetUID();
	ImNodeEd::BeginNode(currentImNodeId);
	ImGui::PushID(static_cast<int>(uidAwareNode->GetUID()));

	ImVec2 cursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos({ cursorPos.x, cursorPos.y + 2 });

	const float bodyMinWidth = leftMinSize.x + rightMinSize.x + 16;
	const ImVec2 nodePinTableSize = { headerTextSize.x > bodyMinWidth ? headerTextSize.x : bodyMinWidth, 0 };

	// Node Header
	ImGui::BeginTable("nodeHeader", 2, ImGuiTableFlags_SizingFixedFit, nodePinTableSize);
	ImGui::TableNextColumn();
	RenderNodeHeader(aNode, nodePinTableSize);
	ImGui::EndTable();

	const ImVec2 headerRectMin = ImGui::GetItemRectMin();
	const ImVec2 headerRectMax = ImGui::GetItemRectMax();
	const ImRect nodeHeaderRect(headerRectMin, headerRectMax);

	// Node Pins
	ImGui::BeginTable("body", 2, ImGuiTableFlags_SizingStretchProp, nodePinTableSize);
	ImGui::TableNextRow();
	ImGui::PushFont(mySettings->NodeBodyFont);
	const size_t numRows = inputPins.size() > outputPins.size() ? inputPins.size() : outputPins.size();
	for (size_t row = 0; row < numRows; row++)
	{
		ImGui::TableNextColumn();
		if (row < inputPins.size())
		{
			const GraphPinClass* inputPin = inputPins[row];
			RenderPin(*inputPin);
		}
		ImGui::TableNextColumn();
		if (row < outputPins.size())
		{
			const GraphPinClass* outputPin = outputPins[row];
			RenderPin(*outputPin);
		}
	}
	ImGui::PopFont();
	ImGui::EndTable();

	const ImVec2 nodeBodyMin = ImGui::GetItemRectMin();
	const ImVec2 nodeBodyMax = ImGui::GetItemRectMax();
	const ImRect nodeBodyRect(nodeBodyMin, nodeBodyMax);
	ImNodeEd::EndNode();

	// This cannot happen before ImNodeEd::EndNode() since the DrawList won't be available.
	RenderNodeStyle(aNode, nodeHeaderRect, nodeBodyRect);
	
	ImGui::PopID();

	ImNodeEd::PopStyleVar();
	ImNodeEd::PopStyleVar();	
}

GraphEditorMethod(void)::RenderNodeHeader(const std::shared_ptr<GraphNodeClass>& aNode, [[maybe_unused]] const ImVec2& aNodeHeaderRect)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 2.0f);
	ImGui::PushFont(mySettings->NodeTitleFont);
	ImGui::TextUnformatted(aNode->GetNodeTitle().data());
	ImGui::PopFont();
}

GraphEditorMethod(void)::RenderNodeStyle([[maybe_unused]] const std::shared_ptr<GraphNodeClass>& aNode, [[maybe_unused]] const ImRect& aNodeHeaderRect, [[maybe_unused]] const ImRect& aNodeBodyRect)
{
	// No styling by default.
}

GraphEditorMethod(void)::RenderPin(const GraphPinClass& aPin)
{
	const PinDirection pinDir = aPin.GetPinDirection();
	const float iconSize = 24.0f;
	const ImVec2 iconSizeRect(iconSize, iconSize);

	if(pinDir == PinDirection::Input)
	{
		ImNodeEd::BeginPin(aPin.GetUID(), static_cast<ImNodeEd::PinKind>(pinDir));

		const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		const ImRect sizeRect(cursorPos, cursorPos + iconSizeRect);
		
		if(ImGui::IsRectVisible(iconSizeRect))
		{
			RenderPinIcon(aPin, sizeRect);
		}

		ImGui::Dummy(iconSizeRect);

		ImNodeEd::EndPin();
		ImGui::SameLine();

		const float y = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(y + 3);
		ImGui::TextUnformatted(aPin.GetLabel().data());
		ImGui::SetCursorPosY(y);
	}
	else // Output
	{
		ImGui::SetCursorPosX(
			ImGui::GetCursorPosX()
			+ ImGui::GetColumnWidth()
			- ImGui::CalcTextSize(aPin.GetLabel().data()).x
			- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x
			- 14.0f
		);

		const float y = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(y + 3);
		ImGui::TextUnformatted(aPin.GetLabel().data());
		ImGui::SameLine();
		ImGui::SetCursorPosY(y);

		ImNodeEd::BeginPin(aPin.GetUID(), static_cast<ImNodeEd::PinKind>(pinDir));

		const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		const ImRect sizeRect(cursorPos, cursorPos + iconSizeRect);
		
		if(ImGui::IsRectVisible(iconSizeRect))
		{
			RenderPinIcon(aPin, sizeRect);
		}

		ImGui::Dummy(iconSizeRect);
		ImNodeEd::EndPin();
	}
}

GraphEditorMethod(void)::RenderPinIcon(const GraphPinClass& aPin, const ImRect& aPinRect)
{
	const GraphColor emptyColor(32, 32, 32, 255);
	const bool pinConnected = aPin.IsConnected();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	GraphEditorUtils::DrawIcon(drawList, aPinRect, GraphIcon::Circle, pinConnected, GraphColor::White, emptyColor);
}

GraphEditorMethod(void)::RenderEdge(
	const GraphEdgeClass& anEdge)
{
	const GraphColor White = GraphColor::White.AsNormalized();
	ImNodeEd::Link(anEdge.EdgeId, anEdge.FromId, anEdge.ToId, {White.R, White.G, White.B, White.A}, 1.0f);
}

#undef GraphEditorMethod

#include <iostream>
#include <windows.h>
#include <shobjidl.h> 
#include <filesystem>

inline std::filesystem::path GetFile()
{
	HRESULT hr;
	IFileDialog* pfd = nullptr;

	// Create the FileOpenDialog object.
	hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Set the options on the dialog.
		DWORD dwOptions;
		hr = pfd->GetOptions(&dwOptions);
		if (SUCCEEDED(hr))
		{
			hr = pfd->SetOptions(dwOptions | FOS_OVERWRITEPROMPT |FOS_CREATEPROMPT);
		}

		// Show the dialog
		hr = pfd->Show(nullptr);
		if (SUCCEEDED(hr))
		{
			// Get the result
			IShellItem* psiResult;
			hr = pfd->GetResult(&psiResult);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFilePath = nullptr;
				hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
				if (SUCCEEDED(hr))
				{
					std::filesystem::path folderPath = pszFilePath;
					CoTaskMemFree(pszFilePath);
					psiResult->Release();
					pfd->Release();
					if(!std::filesystem::exists(folderPath))
					{
						std::filesystem::create_directories(folderPath.parent_path());
					}
					return folderPath;
				}
				psiResult->Release();
			}
		}
		pfd->Release();
	}
	return {};
}