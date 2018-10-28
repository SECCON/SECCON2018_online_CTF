# frozen_string_literal: true

class AdminController < ApplicationController
  before_action :authenticate!

  def index; end

  private

  def authenticate!
    unless session[:logined]
      redirect_to new_admin_session_path
    end
  end
end
