#pragma once

#include "GraphEditor/GraphEditorBase.h"
#include "ScriptGraphEditorTypes.h"
#include <wrl/client.h>

#include "Internal/json.hpp"

class ScriptGraphSchema;
class ScriptGraph;
class ScriptGraphNode;
struct ScriptGraphEdge;
class ScriptGraphPin;
struct ID3D11ShaderResourceView;
struct ID3D11Device;



struct ScriptGraphEditorSettings : public GraphEditorSettingsBase
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> NodeHeaderTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> NodeGradientTexture;

	ImFont* CompactNodeTitleFont = nullptr;

	struct NodeTypeSettings
	{
		unsigned Type;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Icon;
		GraphColor Color;
		// Only has an effect on Variable nodes.
		bool UseTypeColor = false;
		bool CenterTitle = false;
	};

	std::unordered_map<unsigned, NodeTypeSettings> NodeSettings;

	ScriptGraphEditorSettings(const Microsoft::WRL::ComPtr<ID3D11Device>& aDevice);
};

struct ScriptGraphEditorState : public GraphEditorStateBase
{
	bool ShowFlow = false;

	struct VariableEditorState
	{
		int CurrentVariable = -1;
		std::string VarToDelete;
		std::string NewVarName;
		int NewVarTypeIndex = -1;
		TypedDataContainer NewVariableDefault;
	} VariableEditor;

	struct GraphErrorState
	{
		bool HasError = false;
		std::string Message;
		size_t Node;
	} GraphError;

	GraphEditorContextMenuCategory VariablesContextCategory;

	std::vector<std::string> VisibleEditorTypes;
};

class ScriptGraphEditor : public GraphEditorBase<ScriptGraph, ScriptGraphNode, ScriptGraphEdge, ScriptGraphPin, ScriptGraphSchema>
{
public:
	ScriptGraphEditor(ScriptGraphEditorSettings* aSettings, ScriptGraphEditorState* aState, ScriptGraph* aGraph);

	std::vector<uint8_t> TEMP_SAVE_LOAD_dataBlock;
	void Update(float aDeltaTime) override;
protected:

	void RenderEditorStyle() override;
	void RenderNode(const std::shared_ptr<ScriptGraphNode>& aNode) override;
	void RenderNodeHeader(const std::shared_ptr<ScriptGraphNode>& aNode, const ImVec2& aNodeHeaderRect) override;
	void RenderNodeStyle(const std::shared_ptr<ScriptGraphNode>& aNode, const ImRect& aNodeHeaderRect, const ImRect& aNodeBodyRect) override;
	void RenderPin(const ScriptGraphPin& aPin) override;
	void RenderPinIcon(const ScriptGraphPin& aPin, const ImRect& aPinRect) override;
	void RenderEdge(const ScriptGraphEdge& anEdge) override;
	void RenderToolbar() override;
	void RenderUserBackgroundContextMenu() override;

	void HandleUserTransaction() override;

	std::string_view GetEditorTitle() const override;
	std::string_view GetEditorToken() const override;

	std::vector<GraphEditorStateBase::ContextSearchInfo::SearchMenuItem> HandleContextMenuSearch(std::string_view aSearchQuery) override;
	void HandleBackgroundContextMenuItemClicked(const GraphEditorContextMenuItem& aItem) override;

	void ReportError(const ScriptGraphBase* aGraph, size_t aNodeId, std::string_view aErrorMessage);

private:

	void UpdateVariablesContextMenu() const;
	void ScriptGraphEditor_TriggerEntryPointDialog();
	void ScriptGraphEditor_EditVariablesDialog() const;
	void CopyNodes();
	void CutNodes();
	void Paste();

	void InitAutoSave();

	nlohmann::json myCopyObject;
	float myAutoSaveTimer;
	float myAutoSaveDelay;
	int myNextSaveIndex;
	bool myHasAutoSaved;
	bool inputExec = false;
	std::array<std::string, 5> myAuoSaves;
};
