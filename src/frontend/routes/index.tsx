import { 
  createRootRoute, 
  createRoute, 
  createRouter,
  Outlet
} from '@tanstack/react-router';
import { MainLayout } from '../components/layouts/MainLayout';
import { HomePage } from '../components/pages/HomePage';
import { ZenPage } from '../components/pages/ZenPage';

const rootRoute = createRootRoute({
  component: MainLayout
});

const indexRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: '/',
  component: ZenPage
});

const debugRoute = createRoute({
  getParentRoute: () => rootRoute,
  path: '/debug',
  component: HomePage
});

const routeTree = rootRoute.addChildren([indexRoute, debugRoute]);

export const router = createRouter({ routeTree });

declare module '@tanstack/react-router' {
  interface Register {
    router: typeof router;
  }
}
