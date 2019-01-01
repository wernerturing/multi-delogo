/*
 * Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MDL_EDIT_ACTION_H
#define MDL_EDIT_ACTION_H

#include <memory>
#include <string>

#include "filter-generator/Filters.hpp"


namespace mdl {
  class Coordinator;


  class EditAction
  {
  protected:
    virtual ~EditAction() { };

  public:
    virtual void execute(Coordinator& coordinator) = 0;
    virtual void undo(Coordinator& coordinator) = 0;
    virtual std::string get_description() const = 0;
  };


  typedef std::shared_ptr<EditAction> edit_action_ptr;


  class AddFilterAction : public EditAction
  {
  public:
    AddFilterAction(int start_frame, fg::filter_ptr filter);

    void execute(Coordinator& coordinator) override;
    void undo(Coordinator& coordinator) override;
    std::string get_description() const override;

  private:
    int start_frame_;
    fg::filter_ptr filter_;
  };


  class UpdateFilterAction : public EditAction
  {
  public:
    UpdateFilterAction(int start_frame, fg::filter_ptr old_filter, fg::filter_ptr new_filter);

    void execute(Coordinator& coordinator) override;
    void undo(Coordinator& coordinator) override;
    std::string get_description() const override;

  protected:
    int start_frame_;
    fg::filter_ptr old_filter_;
    fg::filter_ptr new_filter_;
  };


  class ChangeFilterTypeAction : public UpdateFilterAction
  {
  public:
    ChangeFilterTypeAction(int start_frame, fg::filter_ptr old_filter, fg::filter_ptr new_filter);

    std::string get_description() const override;
  };


  class RemoveFilterAction : public EditAction
  {
  public:
    RemoveFilterAction(int start_frame, fg::filter_ptr filter);

    void execute(Coordinator& coordinator) override;
    void undo(Coordinator& coordinator) override;
    std::string get_description() const override;

  private:
    int start_frame_;
    fg::filter_ptr filter_;
  };


  class ChangeStartFrameAction : public EditAction
  {
  public:
    ChangeStartFrameAction(int old_start_frame, int new_start_frame, fg::filter_ptr previous_filter);

    void execute(Coordinator& coordinator) override;
    void undo(Coordinator& coordinator) override;
    std::string get_description() const override;

  private:
    int old_start_frame_;
    int new_start_frame_;
    fg::filter_ptr previous_filter_;
  };


  class ShiftAction : public EditAction
  {
  public:
    ShiftAction(int start, int end, int amount);
    void execute(Coordinator& coordinator) override;
    void undo(Coordinator& coordinator) override;
    std::string get_description() const override;

  private:
    int start_;
    int end_;
    int amount_;
    int actual_start_;
    int actual_end_;
  };
}

#endif // MDL_EDIT_ACTION_H
