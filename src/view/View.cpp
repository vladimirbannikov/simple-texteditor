#include "View.h"
#include "../Adapter.h"

View::View(Model *model) {
    model_ = model;
    model->AddObserver(this);
}

void View::update() {
    PrintPage();
}

void View::PrintPage() {
    int y = 0;
    if (model_->PrintWholePage()) {
        Adapter::ClearPage();
        Adapter::RefreshPage();

        auto strings = model_->GetPage().GetStrings();
        auto nums = model_->GetPage().GetNums();

        for (int i = 0; i < strings.size(); i++) {
            if (!model_->IsNumberingMode()) {
                Adapter::Move(y, 0);
                Adapter::Printw(strings[i].c_str());
                y++;
            } else {
                Adapter::Move(y, 0);
                Adapter::Printw(to_string(nums[i]).c_str());

                //printw((to_string(nums[i]) +
                //std::string(model_->GetNumsPanelSize() + 1 - to_string(nums[i]).size(), ' ') + strings[i]).c_str());

                move(y, model_->GetNumsPanelSize() + 1);
                Adapter::Move(y, model_->GetNumsPanelSize() + 1);
                Adapter::Printw(strings[i].c_str());
                y++;
            }
        }
    }
    if (model_->IsCommandMode()) {
        Adapter::Move(model_->GetPage().GetWinSize().first, 0);
        Adapter::Printw(model_->GetPage().GetCommandString().c_str());
    }

    Adapter::Move(model_->GetPage().GetMaxWinSize().first - 1, 0);
    clrtoeol();
    Adapter::ClrToEol();
    Adapter::Printw(model_->GetStatusBar().c_str());

    //move(model_->GetPage().GetCursorPos().first, model_->GetPage().GetCursorPos().second);

    if (model_->IsNumberingMode() && !model_->IsCommandMode() ||
    model_->IsNumberingMode() && model_->IsFindMode())
        Adapter::Move(model_->GetPage().GetCursorPos().first,
                      model_->GetPage().GetCursorPos().second + model_->GetNumsPanelSize() + 1);
    else
        Adapter::Move(model_->GetPage().GetCursorPos().first, model_->GetPage().GetCursorPos().second);
}





