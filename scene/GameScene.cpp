#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	// 3Dモデルの生成
	model_ = Model::Create();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	// デバックカメラの生成
	debugCamera_ = new DebugCamera(600, 400);
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方表示が参照するビュープロジェクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
	// ライン描画参照するビュープロジェクションを指定する（アドレス渡し）
	PrimitiveDrawer::GetInstance()->SetViewProjection(&debugCamera_->GetViewProjection());

	Matrix4 matIdentity;
	matIdentity.m[0][0] = 1;
	matIdentity.m[1][1] = 1;
	matIdentity.m[2][2] = 1;
	matIdentity.m[3][3] = 1;

	// X,Y,Z方向のスケーリングを設定
	worldTransform_.scale_ = { 5, 1, 1 };
	// スケーリング行列を宣言
	Matrix4 matScale;
	// スケーリング倍率を行列に設定する
	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[3][3] = 1;

	worldTransform_.matWorld_ = matIdentity;
	worldTransform_.matWorld_ *= matScale;

	// 行列の転送
	worldTransform_.TransferMatrix();

	// X, Y, Z軸周りの回転角を設定
	worldTransform_.rotation_ = { 0.0f, 0.0f, 3.14f / 4.0f };
	// Z軸回転行列を宣言
	Matrix4 matRotZ = matIdentity;

	// Z軸回転行列の各要素を設定する
	matRotZ.m[0][0] = cos(worldTransform_.rotation_.z);
	matRotZ.m[1][1] = sin(worldTransform_.rotation_.z);
	matRotZ.m[2][2] = -sin(worldTransform_.rotation_.z);
	matRotZ.m[3][3] = cos(worldTransform_.rotation_.z);

	worldTransform_.matWorld_ = matIdentity;
	worldTransform_.matWorld_ *= matRotZ;

	// 行列の転送
	worldTransform_.TransferMatrix();
}

void GameScene::Update() {
	// デバックカメラの更新
	debugCamera_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデル描画
	//model_->Draw(worldTransform_, viewProjection_, textureHandle_);
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
	//PrimitiveDrawer::GetInstance()->DrawLine3d({ -5, -5, -5 }, { 5, 5, 5 }, { 0.5f, 0.5f, 1.0f, 1.0f });

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}