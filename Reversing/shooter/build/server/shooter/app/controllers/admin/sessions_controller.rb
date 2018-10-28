# frozen_string_literal: true

class Admin::SessionsController < ApplicationController
  def new; end

  def create
    if Manager.where(login_id: params[:login_id]).where("password_hash = LCASE(MD5('#{params[:password]}'))").exists?
      session[:logined] = true
      redirect_to admin_root_path
      return
    end
    redirect_to new_admin_session_path
  end
end
