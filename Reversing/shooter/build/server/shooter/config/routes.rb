Rails.application.routes.draw do
  namespace :admin do
    root to: 'dashboards#index'
    resources :users, only: %i[index]
    resource :system_config, only: %i[show]
    resources :sessions, only: %i[new create]
  end

  namespace :api do
    namespace :v1 do
      resources :scores, only: %i[create]
    end
  end
end
